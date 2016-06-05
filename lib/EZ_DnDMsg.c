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
#define _EZ_DND_MSG_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void EZ_HandleDnDMessage MY_ANSIARGS((char *data, unsigned long length));
void EZ_SendDnDMessage MY_ANSIARGS((int type, char *msg, int len, int needFree));
void EZ_BroadcastDnDMessage MY_ANSIARGS((int type, char *msg, int len, int needFree));
void EZ_GenerateDragIsDropedMessage MY_ANSIARGS((char **msg_ret, int *len_ret));
void EZ_GenerateDragStartedMessage MY_ANSIARGS((char **msg_ret, int *len_ret));
void EZ_DnDGetTargetTypesFromMessage MY_ANSIARGS((EZ_DnDMessage *theMsg,
						  Atom *atoms,
						  unsigned int *cont));
void EZ_IWorkAreaDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));
void EZ_ITreeDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));
void EZ_IFListBoxDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));

void EZ_GetDnDDropLocation MY_ANSIARGS((int *x, int *y, int *xx, int *yy));
void EZ_GetDnDDragSrcXYOffset MY_ANSIARGS((int *x, int *y));
EZ_DnDDataEncoder *EZ_FindSpecialEncoder MY_ANSIARGS((void));

void EZ_HandleDNDSameClient MY_ANSIARGS((void));

/*********************************************************************/
static int EZ_WidgetIsARealDnDTarget MY_ANSIARGS(( EZ_Widget *widget));
static int EZ_ItemIsARealDnDTarget  MY_ANSIARGS(( EZ_Item *item));
/*********************************************************************
 *
 * Send a message to a given application. Caller should leave the 
 * first ??? bytes of data free. This func inserts the addresses 
 * there.
 */
/*
void display_message(data, length)
     char *data;
     int length;
{
  int i;
  
  if(length > 0)
    {
      for(i = 0; i < length; i++)
        {
	  int c = data[i];
          if(isprint(c) ) putchar(c);
          else putchar('+');
        }
      printf("]\n");
    }
}
*/
void EZ_SendDnDMessage(type, message, length, needFree)
     int   type;
     char *message;
     int length, needFree;
{
  int  (*OldErrorHandler)();
  Window receiver;
  EZ_EncodeDnDMessageHeader(type, message, length, &receiver);  /* attach addresses */

  if(receiver)
    {
      EZ_XErrorCode = 0;
      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
      EZ_GrabServer();   /* must do this !!!! */
      /* the header first */
      XChangeProperty(EZ_Display,
		      receiver,
		      EZ_DND_MESSAGE_ATOM,
		      XA_STRING, 8,
		      PropModeAppend,
		      (unsigned char *)(EZ_DnDInfo.theHeader),
		      EZ_DND_MSG_HEADER_SIZE
		      );
      /* now append the real message */
      if(length > 0) 
	{
	  /* chop message into chunks if it is too long */
	  int i, nchunks = length/(int)EZ_XServerDataReqSize;
	  char *ptr = message;
	  int   len = 0;
	  for(i = 0; i <= nchunks; i++)
	    {
	      XChangeProperty(EZ_Display,
			      receiver,
			      EZ_DND_MESSAGE_ATOM,
			      XA_STRING, 8,
			      PropModeAppend,
			      (unsigned char *)ptr,
			      (i < nchunks ? EZ_XServerDataReqSize :
			       (length - len))
			      );
	      len += EZ_XServerDataReqSize;
	      ptr += EZ_XServerDataReqSize;
	    }
	}
      EZ_UngrabServer();
      XSync(EZ_Display, False);
      XSetErrorHandler(OldErrorHandler);   
    }
  if(needFree && length > 0) (void) my_free((char *)message);
}

/*********************************************************************
 *
 *  Broadcast a message to all EZWGL applications.
 *  the messages are quite short. !!!
 */
void EZ_BroadcastDnDMessage(type, message, length, needFree)
     int  type;
     char *message;
     int length, needFree;
{
  EZ_ApplRoster *roster;
  Window         window;
  char           *p, *q;
  int  (*OldErrorHandler)();
  (void)EZ_EncodeDnDMessageHeader(type, message, length, &window);
  roster = EZ_OpenEZWGLRoster(0);  /* grab the server remoced 5-19-97 */
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      
      if(sscanf(q,"%lx",&window) == 1)
	{
	  if(EZ_VerifyExistence(window))
	    {
	      /*
	       * Normally, broadcasted msg has just the header,
	       * with the minimal info encoded in.
	       */
	      EZ_XErrorCode = 0;
	      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
	      XChangeProperty(EZ_Display,
			      (Window)window,
			      EZ_DND_MESSAGE_ATOM,
			      XA_STRING, 8,
			      PropModeAppend,
			      (unsigned char *)(EZ_DnDInfo.theHeader),
			      EZ_DND_MSG_HEADER_SIZE
			      );
	      /* in case there are something, has to be a short message !! */
	      if(length > 0 && EZ_XErrorCode == 0)
		{
		  XChangeProperty(EZ_Display,
				  (Window) window,
				  EZ_DND_MESSAGE_ATOM,
				  XA_STRING, 8,
				  PropModeAppend,
				  (unsigned char *)message,
				  length);
		}
	      XSync(EZ_Display, False);
	      XSetErrorHandler(OldErrorHandler);  
	    }
	}
    }
  EZ_CloseEZWGLRoster(roster);
  XSync(EZ_Display,False);

  if(needFree && length > 0) (void) my_free((char *)message);
}

/*************************************************************************************
 *
 * list conversion targets of drag src
 */
void EZ_GenerateDragStartedMessage(message_ret, length_ret)
     char **message_ret;    /* need to be freed ! */
     int  *length_ret;      /* length return */
{
  unsigned int itmp, count = 0;
  int   action_type = EZ_DnDInfo.actionType;
  Atom  *atoms = EZ_DnDInfo.srcTargets;
  EZ_Item *item = NULL;
  EZ_Widget *widget = NULL;
  EZ_DnDDataEncoder *ten, *encoders = NULL;

  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
    {
      item = (EZ_Item *)EZ_DnDInfo.srcItem;
      if(item && EZ_LookupItemFromHT(item))
	encoders = EZ_ItemDnDDataEncoders(item);
      else item = NULL;
    }
  else  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
    {
      widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
      if(widget && EZ_LookupWidgetFromAllHT(widget))
	encoders = EZ_WidgetDnDDataEncoders(widget);
      else widget = NULL;
    }
  ten = encoders;
  while(ten && count < EZ_DND_MAX_TARGETS)
    {
      if(ten->encoder &&  ten->tag == action_type)
	{ atoms[count] = ten->type; count++;}
      ten = ten->next;
    }
  EZ_DnDInfo.srcNTargets = count;

  /* allocate message */
  itmp =  18 * (count + 2) ;
  *length_ret = itmp * sizeof(char);
  *message_ret = (char *)my_malloc(((unsigned int)( 1 + (*length_ret))),_DND_DROP_MESSAGE_);
  if(!(*message_ret)) EZ_OutOfMemory("EZ_GenerateDragIsDropedMessage");
  /* and encode the message */
  {
    int i;
    char *tmp = *message_ret;
    sprintf(tmp, "%x %x", count, action_type);
    tmp = *message_ret + 16;
    for(i = 0; i < count; i++)
      {
	sprintf(tmp, "%lx ", atoms[i]);
	tmp += 18;
      }
  }
}
/*********************************************************************
 *
 *  Called when a drag is released to send a drop message to
 *  all ezwgl applications. 
 *
 *  message_ret is mallocated. has to be freed by the caller.
 */
void EZ_GenerateDragIsDropedMessage(message_ret, length_ret)
     char **message_ret;   /* message return. Message is malloced, so need to be freed */
     int *length_ret;      /* length return */
{
  unsigned int count = 0;
  int   action_type = EZ_DnDInfo.actionType;
  Atom  *atoms = EZ_DnDInfo.srcTargets;
  EZ_Item *item = NULL;
  EZ_Widget *widget = NULL;
  void *obj = NULL;

  EZ_DnDDataEncoder *ten, *encoders = NULL;
  int dmLength = 0, dmNeedFree = 0, itmp;
  char *dmessage = NULL;

  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
    {
      item = (EZ_Item *)EZ_DnDInfo.srcItem;
      if(item && EZ_LookupItemFromHT(item))
	encoders = EZ_ItemDnDDataEncoders(item);
      else item = NULL;
    }
  else  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
    {
      widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
      if(widget && EZ_LookupWidgetFromAllHT(widget))
	encoders = EZ_WidgetDnDDataEncoders(widget);
      else widget = NULL;
    }

  if(item)   {EZ_ItemBlocked(item) += 1; obj = item;}
  if(widget) {EZ_WidgetBlocked(widget) += 1; obj = widget;}

  ten = encoders;
  while(ten && count < EZ_DND_MAX_TARGETS)
    {
      if(ten->encoder &&  ten->tag == action_type)
	{ atoms[count] = ten->type; count++;}
      ten = ten->next;
    }
  EZ_DnDInfo.srcNTargets = count;

  /* if dnd src has filters (encoders for Atom EZ_DND_DRAG_START_ATOM), invoke it now */
  {
    EZ_DnDDataEncoder *sencoder = EZ_FindEncoderGivenTarget(encoders, EZ_DND_DRAG_START_ATOM);
    
    if(sencoder)
      {
	if(sencoder->encoder)
	  (sencoder->encoder)(obj, sencoder->data, &dmessage,  &dmLength, &dmNeedFree);
	if((sencoder->callback).callback)   
	  ((sencoder->callback).callback)(sencoder->object,(sencoder->callback).data);
      }
  }
  if(EZ_DnDInfo.srcNTargets > EZ_DND_MAX_TARGETS)
    EZ_DnDInfo.srcNTargets = EZ_DND_MAX_TARGETS;
  
  count = EZ_DnDInfo.srcNTargets;
  /* allocate message */
  itmp =  18 * (count + 2) ;
  *length_ret = (dmLength + itmp ) * sizeof(char);
  *message_ret = (char *)my_malloc(((unsigned int)( 1 + (*length_ret))),_DND_DROP_MESSAGE_);
  if(!(*message_ret)) EZ_OutOfMemory("EZ_GenerateDragIsDropedMessage");
  /* and encode the message */
  {
    int i;
    char *tmp = *message_ret;
    sprintf(tmp, "%x %x", count, action_type);
    tmp = *message_ret + 16;
    for(i = 0; i < count; i++)
      {
	sprintf(tmp, "%lx ", atoms[i]);
	tmp += 18;
      }
  }
  if(dmLength > 0 && dmessage)
    {
      memcpy( (void *)((*message_ret + itmp)), (void *)dmessage, (size_t)dmLength);
      EZ_DnDInfo.dropMessage = (char *)my_realloc(EZ_DnDInfo.dropMessage, (dmLength + 2)*sizeof(char), 
						  _MISC_DATA_);
      (void)memcpy((void *)EZ_DnDInfo.dropMessage, (void *)dmessage, (size_t)dmLength);
      EZ_DnDInfo.dropMessage[dmLength]='\0';      
      EZ_DnDInfo.dropMessageLength = dmLength;
      if(dmNeedFree) my_free((void *)dmessage);
    }
}
/************************************************************
 *
 * Find Special Encoder, used for drop on the root window
 */
EZ_DnDDataEncoder *EZ_FindSpecialEncoder()
{
  EZ_DnDDataEncoder *encoders = NULL;
  
  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
    {
      EZ_Item *item = (EZ_Item *)EZ_DnDInfo.srcItem;
      if(item && EZ_LookupItemFromHT(item))
	encoders = EZ_ItemDnDDataEncoders(item);
    }
  else  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
    {
      EZ_Widget *widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
      if(widget && EZ_LookupWidgetFromAllHT(widget))
	encoders = EZ_WidgetDnDDataEncoders(widget);
    }

  while(encoders)
    {
      if(encoders->tag == (~0)) break; 
      encoders = encoders->next;
    }
  return(encoders);
}


void EZ_GetDnDDropLocation(x,y, xx, yy)
     int *x, *y, *xx, *yy;
{
  Window junk, ptWin = (Window)EZ_DnDInfo.ptWin;
  if(EZ_WindowExist(ptWin))
    {
      *x = EZ_DnDInfo.px;
      *y = EZ_DnDInfo.py;
      XTranslateCoordinates(EZ_Display,
			    RootWindow(EZ_Display, EZ_ScreenNum),
			    ptWin, *x, *y, xx, yy, &junk);
    }
}

void EZ_GetDnDDragSrcXYOffset(x,y) int *x, *y;
{
  *x = -EZ_DnDInfo.srcX1;
  *y = -EZ_DnDInfo.srcY1;
}
/************************************************************************************************/
void EZ_DnDGetTargetTypesFromMessage(theMsg,atoms,count)
     EZ_DnDMessage *theMsg;
     Atom *atoms;
     unsigned int *count;
{
  int i, dmLength = 0;
  char *message = theMsg->message + EZ_DND_MSG_HEADER_SIZE;
  char *ptr = message;

  sscanf(ptr, "%x %x", count, &i);
  EZ_DnDInfo.actionType = i;
  ptr = message + 16;
  for(i = 0; i < *count; i++)
    {
      sscanf(ptr, "%lx ", atoms+i);
      ptr += 18;
    }
  dmLength = (theMsg->length - EZ_DND_MSG_HEADER_SIZE - (*count +2 ) * 18);
  if(dmLength < 0) dmLength = 0; 
  EZ_DnDInfo.dropMessage[0] = '\0';
  EZ_DnDInfo.dropMessageLength = dmLength;
  if(dmLength > 0)
    {
      char *dmesg = theMsg->message + (theMsg->length - dmLength);
      EZ_DnDInfo.dropMessage = (char *)my_realloc(EZ_DnDInfo.dropMessage, (dmLength + 2)*sizeof(char), 
						  _MISC_DATA_);
      (void)memcpy((void *)EZ_DnDInfo.dropMessage, (void *)dmesg, (size_t)dmLength);
      EZ_DnDInfo.dropMessage[dmLength]='\0';
    }
}

/*********************************************************************
 *  The DnD message handler. It processes some messages before
 *  dispatch them to the widget DnDHandler ..
 */

void EZ_HandleDnDMessage(data,length)
     char *data;           /* the message, with a EZ_DND_MSG_HEADER_SIZE bytes header */
     unsigned long length; /* the total length of message. */
{
  char *p, *q;
  EZ_DnDMessage theMsg;
  EZ_DnDWidgetListEntry  *slist = EZ_DnDWidgetList.next;
  EZ_DnDWidgetListEntry  *list;

  for(p = data; (p - data) < length;)
    {
      q = p; 
      EZ_DecodeDnDMessageHeader(q, &theMsg);
      p = p + theMsg.length; /* forward to next msg */

      switch(theMsg.type)
	{
	case EZ_DND_DRAG_START:
	  /* this message is not intended for any specific widget.
	   * it is broadcasted by the drag src to inform all possible
	   * interested parties that a drag is in progress. The only
	   * sensable thing the receivers need to do is to initlialize
	   * their DndInfo to prepare for a drop. 
	   */
	  if(slist)  /* response only when the application has a dropSite */
	    {
	      int proceed = 1;
	      if(EZ_DnDInfo.id != 0 && EZ_DnDInfo.id != theMsg.id) /* possibly blocked by a DnD */
		{
		  if(EZ_DnDInfo.ImTarget) /* already a drop target. */
		    {
		      if(EZ_DnDInfo.targetStatus ==  EZ_DROP_WAITING_FOR_DATA)
			{ /* we are waiting for a dragSrc convertion */
			  unsigned long CommWin = EZ_DnDInfo.srcCommWin;
			  unsigned long Win = EZ_DnDInfo.srcWin;
			  if(EZ_DnDInfo.srcApplicationId == EZ_VerifyExistence(CommWin) &&
			     CommWin == EZ_WindowIsDnDSrc((Window)Win))
			    {
			      EZ_SendDnDMessage(EZ_DND_TARGET_REQUEST_UNLOCK, NULL, 0, 0);
			      proceed = 0;
			    }
			  else EZ_DnDFinishDrop();
			}
		    }
		  /* else ; how can this happen ? must be an error !! */
		}
	      
	      if(proceed)  /* 1. initialize EZ_DnDInfo */
		{
		  if(theMsg.fmCommWin != theMsg.toCommWin)
		    {
		      EZ_DnDInfo.id = theMsg.id;
		      EZ_DnDInfo.srcCommWin = theMsg.fmCommWin;
		      EZ_DnDInfo.srcWin = theMsg.fmWin;
		      EZ_DnDInfo.srcItem = theMsg.fmItem; /* item */
		      EZ_DnDInfo.srcApplicationId = theMsg.fmId;
		      EZ_DnDInfo.srcX = theMsg.x;
		      EZ_DnDInfo.srcY = theMsg.y;
		      EZ_DnDInfo.srcW = theMsg.w;
		      EZ_DnDInfo.srcH = theMsg.h;
		      EZ_DnDInfo.srcX1 = theMsg.x1;
		      EZ_DnDInfo.srcY1 = theMsg.y1;
		      EZ_DnDInfo.dragIconType = theMsg.iconType;
		      EZ_DnDInfo.px = theMsg.px;
		      EZ_DnDInfo.px = theMsg.py;
		      EZ_DnDInfo.ptWin = theMsg.ptWin;
		      /* reciver and sender are different applications */
		      {
			int i; unsigned int count;
			Atom *atoms = EZ_DnDInfo.srcTargets;
			char *message = theMsg.message + EZ_DND_MSG_HEADER_SIZE;
			char *ptr = message;
			sscanf(ptr, "%x %x", &count, &i);
			EZ_DnDInfo.srcNTargets = count;
			EZ_DnDInfo.actionType = i;
			ptr = message + 16;
			for(i = 0; i < count; i++)
			  {
			    sscanf(ptr, "%lx ", atoms+i);
			    ptr += 18;
			  }
		      }
		    }
		  EZ_DnDInfo.ImTarget = 1;		  
		  EZ_DnDInfo.targetCommWin = (unsigned long)EZ_DummyWindow;
		  EZ_DnDInfo.targetWin = 0L;
		  EZ_DnDInfo.targetItem = 0L;
		  EZ_DnDInfo.targetStatus = 0;
		  EZ_DnDInfo.targetApplicationId = EZ_ApplicationSetupTime;
		  EZ_DnDInfo.targetNTargets = 0;
		  EZ_DnDInfo.targetDecoder = (EZ_DnDDataDecoder  *)NULL;

		  EZ_DnDInfo.dropMessageLength = 0;
		  
		  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_UNKNOWN;
		  EZ_DnDInfo.site = NULL;
		  EZ_DnDInfo.siteHelpString = NULL;
		  EZ_DnDInfo.siteHelpStatus = 0;
		  
		  /* 2. clear the DnDTmp for all possible drop target widgets */
		  EZ_LastDropSite = (EZ_Widget *) NULL;
		  list = slist;
		  while(list)
		    {
		      EZ_Widget *widget = list->widget;
		      if(widget && EZ_LookupWidgetFromAllHT(widget))
			{  EZ_WidgetDnDTmp(widget) = (void *)NULL; }
		      list = list->next;
		    }
		}
	    } /* if(slist) */
	  break;
	case EZ_DND_TARGET_REQUEST_UNLOCK:  /* target is trying to drop this DnD transaction */
	  {
	    unsigned long oldId = EZ_DnDInfo.id;

	    if(EZ_DnDInfo.ImSrc) 
	      {
		Window toWin = (Window)theMsg.toWin;
		EZ_Widget *widget; 
		
		/* inform target to abort */
		EZ_DnDInfo.id = theMsg.id;
		EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_SRC, EZ_DND_ABORT_TO_TARGET, &theMsg);
		EZ_DnDInfo.id = oldId;
		if(EZ_DnDInfo.id == theMsg.id)
		  {
		    widget = EZ_LookupWidgetFromMappedHT(toWin);
		  
		    /* invoke the private DnDHandler */
		    if(widget && EZ_WidgetDnDHandler(widget))
		      (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		    EZ_AbortDrag();
		    EZ_DnDFinishDrag();
		  }
		/* then unblock myself */
		if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
		  EZ_DnDInfo.id = 0;
	      }
	    else
	      {
		(void) fprintf(stderr, "DnDError: targetRequestUnlock ignored\n");
		EZ_DnDInfo.id = 0;
	      }
	  }
	break;
	case EZ_DND_ABORT_TO_TARGET: /* from src */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      /* unpost help popup */
	      EZ_DnDCancelTargetHelp();
	      
	      if(EZ_DnDInfo.ImTarget)
		{
		  Window toWin = (Window)theMsg.toWin;
		  EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);
		  if(widget && EZ_WidgetDnDHandler(widget))
		    (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		  EZ_DnDFinishDrop();
		  EZ_DnDInfo.id = 0; /* forget about this drag */ 
		}
	      else (void) fprintf(stderr, "DnDError: Abort2Target ignored\n");
	    }
	  break;
	case EZ_DND_SRC_REQUEST_UNLOCK:
	  {
	    /* User has initiated a new drag and it is blocked by this widget.
	     * Abort the current transaction.
	     */
	    unsigned long oldId = EZ_DnDInfo.id;
 
	    if(EZ_DnDInfo.ImTarget)
	      {
		Window toWin = (Window)theMsg.toWin;
		EZ_Widget *widget;

		/* unpost help popup */
		EZ_DnDCancelTargetHelp();
		
		EZ_DnDInfo.id = theMsg.id;
		EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);
		EZ_DnDInfo.id = oldId;
		if(EZ_DnDInfo.id == theMsg.id)
		  {
		    widget = EZ_LookupWidgetFromMappedHT(toWin);
		    if(widget && EZ_WidgetDnDHandler(widget)) 
		      (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		    EZ_DnDFinishDrop();
		  }
		/* then unblock myself */
		if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
		  EZ_DnDInfo.id = 0;
	      }
	    else 
	      { 
		(void) fprintf(stderr, "DnDError: SrcRequestUnlock ignored\n");
		EZ_DnDInfo.id = 0;
	      }
	  }
	break;
	case EZ_DND_ABORT_TO_SRC: /* from target */
	  {
	    /* party has sent an abort message. It will no longer handle
	     * the current DnD transaction. Abort
	     */
	    if(EZ_DnDInfo.id == theMsg.id)
	      {
		if(EZ_DnDInfo.ImSrc)
		  {
		    Window toWin = (Window)theMsg.toWin;
		    EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);
		    if(widget && EZ_WidgetDnDHandler(widget))
		      (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		  
		    EZ_AbortDrag();
		    EZ_DnDFinishDrag();
		    EZ_DnDInfo.id = 0; /* forget about this drag */ 
		  }
		else (void) fprintf(stderr, "DnDError: Abort2Src ignored\n");
	      }
	  }
	break;
	case EZ_DND_DRAG_MOTION:   
	  /* this is a BROADCASTED MESSAGE from a drag in progress. The drag is
	   * being moved by the user. This message is intended to the window
	   * the pointer is currently in.
	   */
	  if(EZ_DnDInfo.id == theMsg.id)  /* we are handling this DnD */
	    {
	      if(slist)  /* response only when the application has a dropSite */
		{
		  Window toWin = (Window)theMsg.ptWin;
		  EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);
		  int newDropSite = 0;
		  int sameSite = 0;

		  /* update ptr location */
		  EZ_DnDInfo.px = theMsg.px;
		  EZ_DnDInfo.py = theMsg.py;
		  EZ_DnDInfo.ptWin = theMsg.ptWin;

		  /* motion unposts help popup */
		  if(EZ_DnDInfo.siteHelpStatus != 0)
		    EZ_DnDCancelTargetHelp();

		  if(widget)
		    {
		      if(EZ_LastDropSite != widget)
			{
			  if(EZ_LastDropSite != NULL)
			    EZ_HighlightDropSiteWidget();
			  if(EZ_WidgetMapped(widget) && EZ_WidgetIsDnDTarget(widget) &&
			     EZ_WidgetIsARealDnDTarget(widget))
			    {
			      EZ_LastDropSite = widget;
			      newDropSite = 1;
			    }
			  else EZ_LastDropSite = NULL;
			}
		      else sameSite = 1;

		      if(EZ_WidgetMapped(widget) && EZ_WidgetDnDHandler(widget))
			(EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		      if( (newDropSite && EZ_LastDropSite) ) /* entering a new drop site */
			{
			  if(EZ_DnDInfo.timerEnabled)
			    {
			      EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_WIDGET;
			      EZ_DnDInfo.site = (void *)(EZ_LastDropSite);
			      EZ_DnDInfo.siteHelpString = EZ_WidgetDnDTargetHelp(EZ_LastDropSite);
			      EZ_RestartTimer(EZ_DnDInfo.timer);
			    }
			  EZ_HighlightDropSiteWidget();
			}
		      else if( sameSite && EZ_LastDropSite == NULL)
			{
			  EZ_LastDropSite = widget;
			  EZ_HighlightDropSiteWidget();
			  EZ_LastDropSite = NULL;
			}
		    }
		  else
		    {
		      if(EZ_LastDropSite != NULL ) 
			EZ_HighlightDropSiteWidget();
		      EZ_LastDropSite = NULL;
		    }
		}
	    }
	  break;	  
	case EZ_DND_DRAG_DROPPED:   /* must from a drag Src */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      EZ_DnDInfo.px = theMsg.px;
	      EZ_DnDInfo.py = theMsg.py;
	      EZ_DnDInfo.ptWin = theMsg.ptWin;

	      /* unposts help popup */
	      EZ_DnDCancelTargetHelp();

	      if(slist)  
		{
		  Window toWin = (Window)theMsg.ptWin;
		  EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);

		  if(widget)
		    {
		      /* decode the targets that the dragSrc can convert */
		      if(theMsg.fmCommWin != theMsg.toCommWin)
			{
			  Atom *atoms = EZ_DnDInfo.srcTargets;
			  unsigned int  count = 0;
			  EZ_DnDGetTargetTypesFromMessage(&theMsg, atoms, &count); 
			  EZ_DnDInfo.srcNTargets = count;
			}
		      /* dispatch message to widget's DnDHandler: the private DnDHandler is
		       * responsible for setting up the following fields in EZ_DnDInfo, 
		       * if not the widget itself.
		       *  EZ_DnDInfo.targetType,
		       *  EZ_DnDInfo.targetItem, 
		       */
		      EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_UNKNOWN;
		      EZ_DnDInfo.targetItem = 0L;

		      EZ_DnDInfo.targetWin = theMsg.toWin;
		      
		      if(EZ_WidgetMapped(widget) && EZ_WidgetDnDHandler(widget))
			(EZ_WidgetDnDHandler(widget))(widget, &theMsg);

		      /* so widget does not contain items that are dropSites */
		      if(EZ_DnDInfo.targetItem == 0L)
			{
			  if(EZ_WidgetIsDnDTarget(widget))
			    {
			      EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_WIDGET;
			      EZ_DnDInfo.targetItem = (unsigned long)widget;
			    }
			}
		      
		      /* match decoders with encoders */
		      EZ_DnDMatchTargetsWithDecoders();
		      /* remove drag under effects */
		      if(EZ_LastDropSite) 
			{
			  EZ_HighlightDropSiteWidget();
			}
		      EZ_LastDropSite = NULL;

		      if(theMsg.fmCommWin != theMsg.toCommWin)
			{
			  /* try the first one */
			  EZ_DnDFindNextDecoder();
			  
			  if(EZ_DnDInfo.targetDecoder)
			    {
			      EZ_DnDInfo.targetStatus = EZ_DROP_WAITING_FOR_DATA;
			      EZ_DnDSendDataConversionRequest();
			    }
			  else
			    {
			      /* no matching target found, abort */
			      EZ_DnDInfo.targetStatus = EZ_DROP_ABORTING;
			      EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);
			      EZ_DnDFinishDrop();
			      if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
				EZ_DnDInfo.id = 0;
			    }
			}
		      else  EZ_HandleDNDSameClient();
		    }
		}
	    }
	  break;
	case EZ_DND_REQUEST_CONVERSION:  /* a target has requested a data conversion */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      /* in general, this is the first message the dragSrc received from
	       * the dropTarget. So let's initialize the target*** in EZ_DnDInfo.
	       */
	      if(theMsg.fmCommWin != theMsg.toCommWin)
		{
		  EZ_DnDInfo.targetWin = theMsg.fmWin;
		  EZ_DnDInfo.targetItem = theMsg.fmItem;
		  /* may be some more info ? */
		}
	      /* convert */
	      {
		Atom target;
		char *str = theMsg.message + EZ_DND_MSG_HEADER_SIZE;
		sscanf(str, "%lx", &target);
		EZ_DnDFindDataEncoder(target);

		if(EZ_DnDInfo.srcEncoder)
		  {
		    char *message = NULL;
		    int  status;
		    int  length = 0;
		    int  needfree = 0;
		    EZ_DnDDataEncoder *theEncoder = EZ_DnDInfo.srcEncoder;

		    EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
		    status = (theEncoder->encoder)(theEncoder->object, theEncoder->data, 
						   &message, &length, &needfree);
		    if(status != EZ_DND_FAILURE && length > 0)
		      EZ_DnDSendConversion(theEncoder->type, message, length);
		    else 
		      EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_SRC, EZ_DND_CONVERSION_FAILED, &theMsg);
		    if(needfree && message) (void) my_free(message);
		  }
		else
		  {
		    EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_SRC, EZ_DND_CONVERSION_REFUSED, &theMsg);
		    EZ_DnDInfo.srcStatus = 0; /* forget about this drag */
		  }
	      }
	    }
	  break;
	case EZ_DND_CONVERSION_FAILED:  /* from src: last data conversion has failed */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      int finished = EZ_DnDTargetTryNextTarget();
	      if(finished)
		{
		  Window toWin = (Window)theMsg.toWin;
		  EZ_Widget *widget;
		  EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);

		  EZ_DnDInfo.targetStatus = EZ_DROP_ABORTING;
		  widget = EZ_LookupWidgetFromMappedHT(toWin);
		  if(widget && EZ_WidgetDnDHandler(widget))
		    (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		  EZ_DnDFinishDrop();
		  if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
		    EZ_DnDInfo.id = 0;  
		}
	      else  EZ_DnDInfo.targetStatus = EZ_DROP_TRY_NEXT_TARGET;
	    }
	  break;
	case EZ_DND_CONVERSION_REFUSED: /* from src */
	  /* src cannot honor our conversion request */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      Window toWin = (Window)theMsg.toWin;
	      EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);

	      EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);

	      EZ_DnDInfo.targetStatus = EZ_DROP_ABORTING;
	      if(widget && EZ_WidgetDnDHandler(widget))
		(EZ_WidgetDnDHandler(widget))(widget, &theMsg);
	      EZ_DnDFinishDrop();
	      if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
		EZ_DnDInfo.id = 0;  
	    }
	  break;
	case EZ_DND_DROP_DONE: /* from target */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {                             /* a drop has succesfully finished, clean up */
	      if(EZ_DnDInfo.ImSrc)
		{
		  Window toWin = (Window)theMsg.toWin;
		  EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);
		  if(widget)
		    {
		      if(EZ_WidgetDnDHandler(widget))
			(EZ_WidgetDnDHandler(widget))(widget, &theMsg);
		    }
		  if(EZ_DnDInfo.ImSrc)
		    {
		      EZ_DnDDataEncoder  *encoder = EZ_DnDInfo.srcEncoder;
		      EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
		      if(encoder && (encoder->callback).callback)
			((encoder->callback).callback)(encoder->object, (encoder->callback).data);
		    }
		  EZ_DnDFinishDrag();
		  EZ_DnDInfo.id = 0; /* forget about this drag */ 
		}
	    }
	  break;
	case EZ_DND_DATA:  /* from src */
	  if(EZ_DnDInfo.id == theMsg.id)
	    {
	      if(EZ_DnDInfo.ImTarget)
		{
		  EZ_DnDDataDecoder *theDecoder = EZ_DnDInfo.targetDecoder;
		  if(theDecoder)
		    {
		      int ok = EZ_DND_FAILURE;
		      if(theMsg.thisAtom == theDecoder->type) /* double check */
			{
			  char *msg = theMsg.message + EZ_DND_MSG_HEADER_SIZE;
			  int   mlen = theMsg.length - EZ_DND_MSG_HEADER_SIZE;

			  msg[mlen] = 0;
			  ok = (theDecoder->decoder) (theDecoder->object,
						      theDecoder->data,
						      msg, mlen);
			}
		      if(ok != EZ_DND_FAILURE)
			{
			  EZ_SendDnDMessage(EZ_DND_DROP_DONE, NULL, 0, 0);  /* done */
			  EZ_DnDInfo.targetStatus = EZ_DROP_EXECUTING_CALLBACK;
			  if((theDecoder->callback).callback)
			    ((theDecoder->callback).callback)(theDecoder->object, 
							      (theDecoder->callback).data);
			  EZ_DnDFinishDrop();
			  if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
			    EZ_DnDInfo.id = 0;
			}
		      else /* decoding failure, try next one if available */ 
			{
			  int finished = EZ_DnDTargetTryNextTarget();
			  if(finished)
			    {
			      EZ_DnDInfo.targetStatus = EZ_DROP_ABORTING;
			      EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);
			      EZ_DnDFinishDrop();
			      if(EZ_DnDInfo.srcCommWin != EZ_DnDInfo.targetCommWin)
				EZ_DnDInfo.id = 0; /* forget about this drag */ 
			    }
			  else EZ_DnDInfo.targetStatus = EZ_DROP_TRY_NEXT_TARGET;
			}
		    }
		  else /* should never be here */
		    {
		      EZ_DnDSendErrorMessage(EZ_DND_SENDER_IS_TARGET, EZ_DND_ABORT_TO_SRC, &theMsg);
		    }
		}
	    }
	  break;
	case EZ_DND_DRAG_CANCELLED:  /* src cancelled the drag */
	  {
	    if(slist)  
	      {
		/* unposts help popup */
		EZ_DnDCancelTargetHelp();
		if(EZ_DnDInfo.ImTarget)
		  {
		    if(EZ_DnDInfo.id == theMsg.id ||
		       (EZ_DnDInfo.id == 0 && theMsg.fmCommWin == theMsg.toCommWin))
		      {
			Window toWin = (Window)theMsg.toWin;
			EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);
			EZ_DnDInfo.targetStatus = EZ_DROP_ABORTING;
			if(widget && EZ_WidgetDnDHandler(widget))
			  (EZ_WidgetDnDHandler(widget))(widget, &theMsg);
			
			EZ_DnDCancelTargetHelp();
			if(EZ_LastDropSite) EZ_HighlightDropSiteWidget();
			EZ_LastDropSite = NULL;
			EZ_DnDFinishDrop();
			EZ_DnDInfo.id = 0; /* forget about this drag */ 
		      }
		  }
	      }
	  }
	break;
	case EZ_DND_SRC_REQUEST_HELP:
	  {
	    if(EZ_DnDInfo.id == theMsg.id && EZ_DnDInfo.siteHelpStatus == 0) /* do it only if ... */
	      {
		if(slist && EZ_DnDInfo.ImTarget)
		  {
		    Window toWin = (Window)theMsg.ptWin;
		    EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);

		    EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_UNKNOWN;
		    if(widget)
		      {
			if(EZ_LastDropSite == widget)
			  {
			    EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_WIDGET;
			    EZ_DnDInfo.site = (void *)(EZ_LastDropSite);
			    EZ_DnDInfo.siteHelpString = EZ_WidgetDnDTargetHelp(EZ_LastDropSite);
			  }
			else if(EZ_WidgetDnDHandler(widget))
			  (EZ_WidgetDnDHandler(widget))(widget, &theMsg);			
		      }
		    EZ_DnDShowTargetHelp();
		  }
	      }
	  }
	break;
	case EZ_DND_SRC_CANCEL_HELP:
	  {
	    EZ_DnDCancelTargetHelp();
	  }
	break;
	case EZ_DND_DRAG_LEFT:  /* this message is only for the 3 special widgets that deal with items */
	  {
	    EZ_DnDCancelTargetHelp();
	    if(slist)
	      {
		if(EZ_DnDInfo.id == theMsg.id && EZ_DnDInfo.ImTarget)
		  {
		    Window toWin = (Window)theMsg.ptWin;
		    EZ_Widget *widget = EZ_LookupWidgetFromMappedHT(toWin);		    

		    if(widget)
		      {
			if(EZ_WidgetDnDHandler(widget))
			  (EZ_WidgetDnDHandler(widget))(widget, &theMsg);			
		      }
		  }
	      }
	  }
	break;
	default: 
	  fprintf(stderr, "EZ_HandleDnDMessage: unknown message type\n");
	  break;
	}
    }
}
/*********************************************************************/
void EZ_HandleDNDSameClient()
{
  int done = 0;
  EZ_DnDDataDecoder *decoder;
  EZ_DnDDataEncoder *encoder;

  while(!done)
    {
      EZ_DnDFindNextDecoder();
      decoder = EZ_DnDInfo.targetDecoder;
      if(decoder)
	{
	  Atom target = decoder->type;
	  int status;
	  int length = 0;
	  int needFree = 0;
	  char *message = NULL;
	  EZ_DnDFindDataEncoder(target);
	  encoder = EZ_DnDInfo.srcEncoder;
	  if(encoder)
	    {
	      EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
	      status = (encoder->encoder)(encoder->object, encoder->data, 
					  &message, &length, &needFree);
	      if(status != EZ_DND_FAILURE && length > 0)
		{
		  char *msg = message;
		  int   mlen = length;
		  int  ok = EZ_DND_FAILURE;

		  ok = (decoder->decoder) (decoder->object,  decoder->data, msg, mlen);
		  if(ok != EZ_DND_FAILURE)
		    {
		      EZ_DnDInfo.targetStatus = EZ_DROP_EXECUTING_CALLBACK;
		      if((decoder->callback).callback)
			((decoder->callback).callback)(decoder->object, (decoder->callback).data);
		      EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
		      if(encoder && (encoder->callback).callback)
			((encoder->callback).callback)(encoder->object, (encoder->callback).data); 
		      done = 1;
		    }
		}
	      if(needFree && message) (void) my_free(message);
	    }
	  if(!done)
	    {
	      int i = 0;
	      int count = EZ_DnDInfo.targetNTargets;  
	      Atom *atoms = EZ_DnDInfo.targetTargets;
	      while(i < count)
		{
		  if(atoms[i] == target) 
		    { atoms[i] = 0; break; }
		  i++; 
		}
	    }
	}
      else { EZ_AbortDrag(); break;}
    }
  EZ_DnDFinishDrag();
  EZ_DnDFinishDrop();
  EZ_DnDInfo.id = 0; 
}
/*********************************************************************
 *
 * Specific DnD message handlers
 *
 **********************************************************************/
/*
 *  DnD Message Handler for widget IWorkArea
 */
void EZ_IWorkAreaDnDHandle(widget,theMsg)
     EZ_Widget     *widget;
     EZ_DnDMessage *theMsg;
{
  switch(theMsg->type)
    {
    case EZ_DND_DRAG_MOTION:    /* must come from a Dnd Src */
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      int x = theMsg->px;
	      int y = theMsg->py;
	      int nitems = EZ_IWorkAreaNItems(widget);
	      if(nitems > 0)
		{
		  Window wwin = EZ_WidgetWindow(widget);
		  EZ_Item *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);
		  EZ_Item *newItem = NULL;
		  EZ_Item **data = EZ_IWorkAreaData(widget);
		  int wox, woy;
		  Window child;
		  
		  XTranslateCoordinates(EZ_Display, wwin,
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  x = x-wox; /* pointer position in wwin, rel to UL corner */
		  y = y-woy;
		  if(x <= 0 || y <= 0 || x >= EZ_WidgetWidth(widget) ||  y >= EZ_WidgetHeight(widget))
		    newItem = NULL;
		  else newItem = EZ_FindPointerOnItemIWA(data, nitems, lastItem, x,y);
		  if(newItem != NULL)
		    {
		      if(!EZ_ItemIsDnDTarget(newItem) || !EZ_ItemIsARealDnDTarget(newItem) ||
			 EZ_ItemType(newItem) == EZ_WIDGET_ITEM)
			newItem = NULL;
		    }
		  
		  EZ_WidgetDnDTmp(widget) = newItem;
		  
		  if(lastItem || newItem)
		    {
		      if(newItem) EZ_LastDropSite = NULL;	
		      
		      if(lastItem != newItem)
			{
			  int Rx,Ry,Rw,Rh;
			  Rx = Ry = EZ_WidgetBorderWidth(widget) +EZ_WidgetPadB(widget);
			  Rw = EZ_WidgetWidth(widget) - (Rx << 1);
			  Rh = EZ_WidgetHeight(widget) - (Ry << 1);
			  if(lastItem) EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
			  if(newItem)
			    {
			      EZ_DnDOutlineItem(widget,newItem, Rx,Ry,Rw,Rh);
			      if(EZ_DnDInfo.timerEnabled)
				{
				  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
				  EZ_DnDInfo.site = (void *)newItem;
				  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(newItem);
				  EZ_RestartTimer(EZ_DnDInfo.timer);
				}
			    }
			}
		    }
		}
	    }
	}
      break;
    case EZ_DND_DRAG_DROPPED:   /* must from a drag Src */
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      EZ_Item *lastItem = EZ_WidgetDnDTmp(widget);

	      if(lastItem) /* a drag is over an item */
		{
		  int Rx,Ry,Rw,Rh;
		  Rx = Ry = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		  Rw = EZ_WidgetWidth(widget) - (Rx << 1);
		  Rh = EZ_WidgetHeight(widget) - (Ry << 1);
		  EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		  /* fillin the target info in EZ_DnDInfo */
		  EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_ITEM;
		  EZ_DnDInfo.targetItem = (unsigned long)lastItem;
		}
	    }
	}
      break;
    case EZ_DND_SRC_REQUEST_HELP:
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      EZ_Item *lastItem = EZ_WidgetDnDTmp(widget);

	      if(lastItem) 
		{
		  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
		  EZ_DnDInfo.site = (void *)lastItem;
		  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(lastItem);
		}
	    }
	}
      break;
    case EZ_DND_DRAG_LEFT: 
    case EZ_DND_DRAG_CANCELLED:  /* src cancelled the drag */
      {
	if(EZ_WidgetMapped(widget))
	  {
	    EZ_Item *lastItem = EZ_WidgetDnDTmp(widget);
	    
	    if(lastItem) /* a drag is over an item */
	      {
		int Rx,Ry,Rw,Rh;
		Rx = Ry = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		Rw = EZ_WidgetWidth(widget) - (Rx << 1);
		Rh = EZ_WidgetHeight(widget) - (Ry << 1);
		EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		EZ_WidgetDnDTmp(widget) = NULL;
	      }
	  }
      }
    break;
    default:
      break;
    }
}
/*******************************************************************************
 *
 *  DnD Message Handler for widget ITree
 */
void EZ_ITreeDnDHandle(widget,theMsg)
     EZ_Widget     *widget;
     EZ_DnDMessage *theMsg;
{
  switch(theMsg->type)
    {
    case EZ_DND_DRAG_MOTION:    /* must come from a Dnd Src */
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      if(EZ_ITreeWRoot(widget) != (EZ_TreeNode *)NULL)
		{
		  int x = theMsg->px;
		  int y = theMsg->py;
		  EZ_Item       *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);   
		  EZ_Item       *newItem = NULL;
		  Window wwin = EZ_WidgetWindow(widget);
		  int wox, woy;
		  Window child;

		  XTranslateCoordinates(EZ_Display, wwin,
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  x = x-wox; /* pointer position in wwin, rel to UL corner */
		  y = y-woy;
		  if( !(x <= 0 || y <= 0 || x >= EZ_WidgetWidth(widget) ||  y >= EZ_WidgetHeight(widget)))
		    {
		      EZ_TreeNode *newNode = EZ_FindPointerOnTreeNode(x,y, EZ_ITreeWRoot(widget));
		      if(newNode)
			{
			  newItem = EZ_TreeNodeItem(newNode);
			  if(newItem)
			    {
			      if(!EZ_ItemIsDnDTarget(newItem) || !EZ_ItemIsARealDnDTarget(newItem) ||
				 EZ_ItemType(newItem) == EZ_WIDGET_ITEM)
				newItem = NULL;
			    }
			}
		    }
		  EZ_WidgetDnDTmp(widget) = (void *)newItem;
		  
		  if(lastItem || newItem)
		    {
		      if(newItem) EZ_LastDropSite = NULL;	
		      
		      if(lastItem != newItem)
			{
			  int Rx,Ry,Rw,Rh;
			  Rx = Ry = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
			  Rw = EZ_WidgetWidth(widget) - (Rx << 1);
			  Rh = EZ_WidgetHeight(widget) - (Ry << 1);
			  if(lastItem) EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
			  if(newItem)
			    {
			      EZ_DnDOutlineItem(widget,newItem, Rx,Ry,Rw,Rh);
			      if(EZ_DnDInfo.timerEnabled)
				{
				  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
				  EZ_DnDInfo.site = (void *)newItem;
				  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(newItem);
				  EZ_RestartTimer(EZ_DnDInfo.timer);
				}
			    }
			}
		    }
		}
	    }
	}
      break;
    case EZ_DND_DRAG_DROPPED:   /* must from a drag Src */
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      EZ_Item *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);
		
	      if(lastItem) /* a drag is over an item */
		{
		  int Rx,Ry,Rw,Rh;
		  Rx = Ry = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		  Rw = EZ_WidgetWidth(widget) - (Rx << 1);
		  Rh = EZ_WidgetHeight(widget) - (Ry << 1);
		  EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		  /* fillin the target info in EZ_DnDInfo */
		  EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_ITEM;
		  EZ_DnDInfo.targetItem = (unsigned long)lastItem;
		}
	    }
	}
      break;
    case EZ_DND_SRC_REQUEST_HELP:
      if(EZ_DnDInfo.id == theMsg->id)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      EZ_Item *lastItem = EZ_WidgetDnDTmp(widget);

	      if(lastItem) 
		{
		  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
		  EZ_DnDInfo.site = (void *)lastItem;
		  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(lastItem);
		}
	    }
	}
      break;
    case EZ_DND_DRAG_LEFT: 
    case EZ_DND_DRAG_CANCELLED:  /* src cancelled the drag */
      {
	if(EZ_WidgetMapped(widget))
	  {
	    EZ_Item *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);
		
	    if(lastItem) /* a drag is over an item */
	      {
		int Rx,Ry,Rw,Rh;
		Rx = Ry = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		Rw = EZ_WidgetWidth(widget) - (Rx << 1);
		Rh = EZ_WidgetHeight(widget) - (Ry << 1);
		EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		EZ_WidgetDnDTmp(widget) = NULL;
	      }
	  }
      }
    break;
    default:
      break;
    }
}
/*******************************************************************************
 *
 *  DnD Message Handler for widget ITree
 */
void EZ_IFListBoxDnDHandle(widget,theMsg)
     EZ_Widget     *widget;
     EZ_DnDMessage *theMsg;
{
  int smode = EZ_IFListBoxSelectionType(widget); /* 0 for picking an item a time  */
  int *vsa = EZ_IFListBoxVSizes(widget);
  if(smode)
    {
      int Rx,Ry,Rw,Rh;
      int sline;
      switch(theMsg->type)
	{
	case EZ_DND_DRAG_MOTION:    /* must come from a Dnd Src */
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget) && EZ_IFListBoxRows(widget) > 0)
		{
		  int x = theMsg->px;
		  int y = theMsg->py;
		  int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		  int yoffset = bw + EZ_WidgetPadY(widget);
		  EZ_Item **newRow=NULL, **lastRow;
		  Window wwin = EZ_WidgetWindow(widget);
		  Window child;
		  int wox, woy;

		  Rx = bw;
		  Ry = yoffset + vsa[0];
		  Rw = EZ_WidgetWidth(widget) - (bw << 1);
		  Rh = EZ_WidgetHeight(widget) - bw - Ry;
		  
		  lastRow = (EZ_Item **)EZ_WidgetDnDTmp(widget);
		  
		  XTranslateCoordinates(EZ_Display, wwin,
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  x = x-wox;
		  y = y-woy;
		  sline = EZ_FindPointerOnRowIFL(EZ_IFListBoxData(widget), 
						 EZ_IFListBoxRows(widget), 
						 EZ_IFListBoxColumns(widget), 
						 x,y, Rx,Ry,Rw,Rh);
		  if(sline > 0)
		    {
		      newRow =  EZ_IFListBoxData(widget)[sline];
		      if(newRow)
			{
			  EZ_Item *item = *newRow;
			  if(item == NULL || !EZ_ItemIsDnDTarget(item) || !EZ_ItemIsARealDnDTarget(item) ||
			     EZ_ItemType(item) == EZ_WIDGET_ITEM)
			    newRow = NULL;
			}
		    }

		  EZ_WidgetDnDTmp(widget) = newRow;
		  
		  if(lastRow || newRow)
		    {
		      if(newRow) EZ_LastDropSite = NULL;	
		      
		      if(lastRow != newRow)
			{
			  if(lastRow) EZ_DnDOutlineRow(widget,lastRow,
						       (int)EZ_IFListBoxColumns(widget),
						       Rx,Ry,Rw,Rh);
			  if(newRow) 
			    {
			      EZ_Item *newItem = newRow[0];
			      EZ_DnDOutlineRow(widget,newRow,
					       (int)EZ_IFListBoxColumns(widget),
					       Rx,Ry,Rw,Rh);
			      if(EZ_DnDInfo.timerEnabled)
				{
				  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
				  EZ_DnDInfo.site = (void *)newItem;
				  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(newItem);
				  EZ_RestartTimer(EZ_DnDInfo.timer);
				}
			    }
			}
		    }
		}
	    }
	  break;
	case EZ_DND_DRAG_DROPPED:   /* must from a drag Src */
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget))
		{
		  EZ_Item **lastRow = (EZ_Item **)EZ_WidgetDnDTmp(widget);
		  
		  if(lastRow) /* a drag is over an item */
		    {
		      int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		      int yoffset = bw + EZ_WidgetPadY(widget);
		      int ww, hh;
		      Rx = bw;
		      Ry = yoffset + vsa[0];
		      ww = EZ_WidgetWidth(widget);
		      Rw = ww - (bw << 1);
		      hh = EZ_WidgetHeight(widget);
		      Rh = hh - bw - Ry;
		      EZ_DnDOutlineRow(widget,lastRow,
				       (int)EZ_IFListBoxColumns(widget),
				       Rx,Ry,Rw,Rh);
		      EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_ITEM;
		      EZ_DnDInfo.targetItem = (unsigned long)(*lastRow);
		    }
		}
	    }
	  break;
	case EZ_DND_SRC_REQUEST_HELP:
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget))
		{
		  EZ_Item **lastRow = (EZ_Item **)EZ_WidgetDnDTmp(widget);
		  if(lastRow) 
		    {
		      EZ_Item *lastItem = lastRow[0];
		      EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
		      EZ_DnDInfo.site = (void *)lastItem;
		      EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(lastItem);
		    }
		}
	    }
	  break;
	case EZ_DND_DRAG_LEFT: 
	case EZ_DND_DRAG_CANCELLED:  /* src cancelled the drag */
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget))
		{
		  EZ_Item **lastRow = (EZ_Item **)EZ_WidgetDnDTmp(widget);
		  
		  if(lastRow) /* a drag is over an item */
		    {
		      int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		      int yoffset = bw + EZ_WidgetPadY(widget);
		      int ww, hh;
		      Rx = bw;
		      Ry = yoffset + vsa[0];
		      ww = EZ_WidgetWidth(widget);
		      Rw = ww - (bw << 1);
		      hh = EZ_WidgetHeight(widget);
		      Rh = hh - bw - Ry;
		      EZ_DnDOutlineRow(widget,lastRow,
				       (int)EZ_IFListBoxColumns(widget),
				       Rx,Ry,Rw,Rh);
		      EZ_WidgetDnDTmp(widget) = NULL;
		    }
		}
	    }
	  break;
	default:
	  break;
	}
    }
  else /* item mode */
    {
      int Rx,Ry,Rw,Rh;
      switch(theMsg->type)
	{
	case EZ_DND_DRAG_MOTION:    /* must come from a Dnd Src */
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget) && EZ_IFListBoxRows(widget) > 0)
		{
		  int x = theMsg->px;
		  int y = theMsg->py;
		  int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		  int yoffset = bw + EZ_WidgetPadY(widget);
		  EZ_Item *newItem=NULL, *lastItem;
		  Window wwin = EZ_WidgetWindow(widget);
		  Window child;
		  int wox, woy;
		  int rowN = -1, colN;

		  Rx = bw;
		  Ry = yoffset + vsa[0];
		  Rw = EZ_WidgetWidth(widget) - (bw << 1);
		  Rh = EZ_WidgetHeight(widget) - bw - Ry;
		  
		  lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);

		  XTranslateCoordinates(EZ_Display, wwin,
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  x = x-wox;
		  y = y-woy;
		  EZ_FindPointerOnRowAndColIFL(EZ_IFListBoxData(widget), 
					       (int)EZ_IFListBoxRows(widget), 
					       (int)EZ_IFListBoxColumns(widget), 
					       x,y, &rowN, &colN,
					       Rx,Ry,Rw,Rh);  
		  if(rowN > 0 && colN >= 0)
		    {
		      EZ_Item ***data, **row;
		      data = EZ_IFListBoxData(widget);
		      row = data[rowN];
		      newItem = row[colN];

		      if(newItem)
			{
			  if(!EZ_ItemIsDnDTarget(newItem) ||  !EZ_ItemIsARealDnDTarget(newItem) ||
			     EZ_ItemType(newItem) == EZ_WIDGET_ITEM)
			    newItem = NULL;
			}

		    }

		  EZ_WidgetDnDTmp(widget) = newItem;
		  
		  if(lastItem || newItem)
		    {
		      if(newItem) EZ_LastDropSite = NULL;	
		      
		      if(lastItem != newItem)
			{
			  if(lastItem) EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
			  if(newItem)
			    {
			      EZ_DnDOutlineItem(widget,newItem, Rx,Ry,Rw,Rh);
			      if(EZ_DnDInfo.timerEnabled)
				{
				  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
				  EZ_DnDInfo.site = (void *)newItem;
				  EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(newItem);
				  EZ_RestartTimer(EZ_DnDInfo.timer);
				}
			    }
			}
		    }
		}
	    }
	  break;
	case EZ_DND_DRAG_DROPPED:   /* must from a drag Src */
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget))
		{
		  EZ_Item *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);
		  
		  if(lastItem) /* a drag is over an item */
		    {
		      int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		      int yoffset = bw + EZ_WidgetPadY(widget);
		      int ww, hh;
		      Rx = bw;
		      Ry = yoffset + vsa[0];
		      ww = EZ_WidgetWidth(widget);
		      Rw = ww - (bw << 1);
		      hh = EZ_WidgetHeight(widget);
		      Rh = hh - bw - Ry;
		      EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		      EZ_DnDInfo.targetType = EZ_DND_OBJ_IS_ITEM;
		      EZ_DnDInfo.targetItem = (unsigned long)lastItem;
		    }
		}
	    }
	  break; 
	case EZ_DND_SRC_REQUEST_HELP:
	  if(EZ_DnDInfo.id == theMsg->id)
	    {
	      if(EZ_WidgetMapped(widget))
		{
		  EZ_Item *lastItem = EZ_WidgetDnDTmp(widget);

		  if(lastItem) 
		    {
		      EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_ITEM;
		      EZ_DnDInfo.site = (void *)lastItem;
		      EZ_DnDInfo.siteHelpString = EZ_ItemDnDTargetHelp(lastItem);
		    }
		}
	    }
	  break;
	case EZ_DND_DRAG_LEFT: 
	case EZ_DND_DRAG_CANCELLED:  /* src cancelled the drag */
	  {
	    if(EZ_WidgetMapped(widget))
	      {
		EZ_Item *lastItem = (EZ_Item *)EZ_WidgetDnDTmp(widget);
		
		if(lastItem) /* a drag is over an item */
		  {
		    int bw = EZ_WidgetBorderWidth(widget)+EZ_WidgetPadB(widget);
		    int yoffset = bw + EZ_WidgetPadY(widget);
		    int ww, hh;
		    Rx = bw;
		    Ry = yoffset + vsa[0];
		    ww = EZ_WidgetWidth(widget);
		    Rw = ww - (bw << 1);
		    hh = EZ_WidgetHeight(widget);
		    Rh = hh - bw - Ry;
		    EZ_DnDOutlineItem(widget,lastItem,Rx,Ry,Rw,Rh);
		    EZ_WidgetDnDTmp(widget) = NULL;
		  }
	      }
	  }
	break;
	default:
	  break;
	}
    }
}
/*********************************************************************************/
static int findAMatchedTarget(decoders) EZ_DnDDataDecoder *decoders;
{
  if(decoders)
    {
      EZ_DnDDataDecoder  *ptr;
      Atom  target, *SrcAtoms = EZ_DnDInfo.srcTargets;
      int    count = EZ_DnDInfo.srcNTargets;
      unsigned int ta_type, action_type = EZ_DnDInfo.actionType;
      int i;

      ptr = decoders;

      while(ptr)
	{
	  target = ptr->type;
	  ta_type = ptr->tag;
	  for(i = 0; i < count; i++)
	    {
	      if( SrcAtoms[i] == target && ta_type == action_type)
		{
		  return(1);
		  break;
		}
	    }
	  ptr = ptr->next;
	}
    }
  return(0);
}
static int EZ_WidgetIsARealDnDTarget(widget) EZ_Widget *widget;
{
  return(findAMatchedTarget(EZ_WidgetDnDDataDecoders(widget)));
}
static int EZ_ItemIsARealDnDTarget(item) EZ_Item *item;
{
  return(findAMatchedTarget(EZ_ItemDnDDataDecoders(item)));
}
/*********************************************************************************/
#undef _EZ_DND_MSG_C_
