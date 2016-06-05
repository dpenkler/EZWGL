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
 ***  Messages: this file implements a one-question one-answer ***
 ***            communication mechanism between EZWGL clients  ***
 ***                                                           ***
 *****************************************************************/

#define _EZ_MESSAGE_C_
#include "EZ_Widget.h"

/******************************************************************/
void  EZ_SetDefaultMessageHandler MY_ANSIARGS((EZ_MessageHandler handler, void *data));
void  EZ_RegisterMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
                                             EZ_MessageHandler handler,
                                             int timeout, EZ_CallBack tcb, void *cdata));
void  EZ_AddMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					EZ_MessageHandler handler, int timeout));
void  EZ_DeleteMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					   EZ_MessageHandler handler));

void  EZ_SendMessage MY_ANSIARGS((Atom mtype, char *message, int length, int msgid,
				  Window recipient, int replayId, int isReply));
void  EZ_BroadcastMessage MY_ANSIARGS((Atom mtype, char *message, int length, 
				       int msgid, int replyId));

void  EZ_ReplyMessage  MY_ANSIARGS((EZ_Message *fromsg, char *reply, int replylength));
void  EZ_ReplyMessageR MY_ANSIARGS((EZ_Message *fromsg, char *reply, int len, int replyId));

/******************************************************************/
void  EZ_HandleGeneralMessage MY_ANSIARGS((char *msg, int length));
void  EZ_InitializeMessageHandlers MY_ANSIARGS((void));

static int   findMsgHandles MY_ANSIARGS((EZ_MessageHandle **ret, Atom atom, int mId));
static int   msgHandleExist MY_ANSIARGS((EZ_MessageHandle *handle));
static void  timerCallback MY_ANSIARGS((EZ_Timer *timer, EZ_MessageHandle *handle));
/******************************************************************/
static EZ_MessageHandle msgHandlerListHead;
/******************************************************************
 *
 *  Register a message handler
 */
void EZ_AddMessageHandler(atom, mId, data, handler, timeout)
     Atom   atom;     void   *data;     int    mId, timeout;  EZ_MessageHandler handler;
{  EZ_RegisterMessageHandler(atom, mId, data, handler, timeout, NULL, NULL);}
/******************************************************************/

void EZ_RegisterMessageHandler(atom, mId, data, handler, timeout, tcb, tcbdata)
     Atom   atom;
     void   *data;
     int    mId, timeout;
     EZ_MessageHandler handler;
     EZ_CallBack tcb; void *tcbdata;
{
  EZ_MessageHandle *entry, *ptr;

  entry = (EZ_MessageHandle *)my_malloc(sizeof(EZ_MessageHandle), _C_MESSAGE_HANDLE_);
  if(entry == NULL) EZ_OutOfMemory("EZ_CreateMessageHandler");

  entry->atom = atom;
  entry->data = data;
  entry->handler = handler;
  entry->messageId = mId;
  entry->next = NULL;
  if(timeout > 0) 
    {
      entry->timerCB = tcb;
      entry->timerData = tcbdata;
      entry->timer = EZ_CreateTimer(timeout, 0, 0, 
                                    (EZ_CallBack) timerCallback, entry, mId);
    }
  else 
    {
      entry->timer = NULL;
      entry->timerCB = NULL;
      entry->timerData = NULL;      
    }

  /* keep the list in order: insert at the end */
  ptr = &msgHandlerListHead;
  while(ptr->next != NULL) ptr = ptr->next;
  ptr->next = entry;
}

/******************************************************************
 *
 *  Delete a message handle
 */
void EZ_DeleteMessageHandler(atom, mId, data, handler)
     Atom   atom;
     void   *data;
     int    mId;
     EZ_MessageHandler handler;
{
  EZ_MessageHandle *parent = NULL, *ptr = msgHandlerListHead.next;
  
  while(ptr)
    {
      if(ptr->atom == atom && ptr->data == data && ptr->messageId == mId &&
	 ptr->handler == handler)
	{
	  if(parent == NULL) msgHandlerListHead.next = ptr->next;
	  else parent->next = ptr->next;
          if(ptr->timer) EZ_CancelTimer(ptr->timer);
	  my_free(ptr);
	  return;
	}
      parent = ptr;
      ptr = ptr->next;
    }     
}

/******************************************************************
 *
 *  Search message handlers  (atom, id)
 */
static int findMsgHandles(h_ret, atom, id)
     Atom atom; int id;
     EZ_MessageHandle **h_ret;
{
  EZ_MessageHandle *ptr = msgHandlerListHead.next;
  int ret = 0;

  while(ptr && ret < 256)
    {
      if(ptr->atom == atom && ptr->messageId == id) { h_ret[ret++] = ptr; }
      ptr = ptr->next;
    }
  return(ret);
}
static int msgHandleExist(handle)
     EZ_MessageHandle *handle;
{
  EZ_MessageHandle *ptr = msgHandlerListHead.next;

  while(ptr)
    {
      if(ptr == handle) return(1);
      ptr = ptr->next;
    }
  return(0);

}
/******************************************************************
 *
 *  Invoke messageHandler timeout callback and delete the handler
 */
static void timerCallback(timer, handle)
     EZ_Timer *timer; EZ_MessageHandle *handle;
{
  if(handle && timer)
    {
      EZ_MessageHandle *parent = NULL, *ptr = msgHandlerListHead.next;

      while(ptr)
	{
	  if(ptr == handle)
	    {
	      if(parent == NULL) msgHandlerListHead.next = ptr->next;
	      else parent->next = ptr->next;
              if(ptr->timerCB) { (ptr->timerCB)(ptr->data, ptr->timerData); }
	      my_free(ptr);
	      return;
	    }
	  parent = ptr;
	  ptr = ptr->next;
	}     
    }
}
/**************************************************************************************
 * 
 *  Set the default message handler
 */
void EZ_SetDefaultMessageHandler(handler, data)
     EZ_MessageHandler handler;
     void *data;
{
  msgHandlerListHead.handler = handler;
  msgHandlerListHead.data = data;
}

/**************************************************************************************
 * 
 *  Initialize/rest message handler list
 */
void EZ_InitializeMessageHandlers()
{
  msgHandlerListHead.handler = NULL;
  msgHandlerListHead.data = NULL;
  msgHandlerListHead.timer = NULL;
  msgHandlerListHead.messageId = 0;
  msgHandlerListHead.next = NULL;
}

void EZ_ResetMessageHandlers()
{
  EZ_MessageHandle *tmp, *ptr = msgHandlerListHead.next;  
  while(ptr)
    {
      tmp = ptr;
      ptr = ptr->next;
      my_free((void *)tmp);
    }
  msgHandlerListHead.handler = NULL;
  msgHandlerListHead.data = NULL;
  msgHandlerListHead.timer = NULL;
  msgHandlerListHead.messageId = 0;
  msgHandlerListHead.next = NULL;
}

/**************************************************************************************
 * 
 *  Invoked by  EZ_HandlePropertyNotify when there is a EZ_MESSAGE_ATOM
 *  property change.
 */
void EZ_HandleGeneralMessage(data, length)
     char *data; int length;
{
  char *ptr = data;
  while(length >= EZ_MSG_HEADER_LENGTH)
    {
      unsigned long mlen, ulmtype, ulmid, ulsrc, ulsrcClass, ulsrcInstance;
      unsigned long ulrmid,  ulisA;
      if(sscanf(ptr, "%lx %lx %lx %lx %lx %lx %lx %lx", &mlen, &ulmtype, &ulmid, 
		&ulsrc, &ulsrcClass, &ulsrcInstance, &ulrmid, &ulisA) == 8)
	{
	  EZ_MessageHandle     *handles[256];
	  EZ_Message           theMsg;
	  int                  i, numHandles;
	  int                  handled = 0;

	  theMsg.messageType   = (Atom) ulmtype;
	  theMsg.messageLength = (int)mlen - EZ_MSG_HEADER_LENGTH;
	  theMsg.message       = ptr + EZ_MSG_HEADER_LENGTH;
	  theMsg.messageId     = (int)ulmid;
	  theMsg.replyId       = (int)ulrmid;
	  theMsg.isReply       = (int)ulisA;
	  
	  theMsg.sender        = (Window)ulsrc;
	  theMsg.senderClass   = (Atom) ulsrcClass;
	  theMsg.senderInstance= (Atom) ulsrcInstance;
	  theMsg.senderIsSelf  = (theMsg.sender == EZ_DummyWindow)? 1 : 0;

          if(theMsg.messageType == EZ_RESOURCE_EDITOR_ATOM)
            handled = EZ_HandleResourceEditorMessage(&theMsg);
          if(handled == 0)
            {
              numHandles = findMsgHandles(&handles[0], theMsg.messageType, theMsg.messageId);
              if(numHandles > 0)
                {
                  for(i = 0; i < numHandles; i++)
                    {
                      if(msgHandleExist(handles[i]) && handles[i]->handler != NULL)
                        {
                          handles[i]->handler(&theMsg, handles[i]->data);
                          handled = 1;
                        }
                    }
                }
              if(handled == 0)
                {
                  if(msgHandlerListHead.handler)
                    msgHandlerListHead.handler(&theMsg, msgHandlerListHead.data);
                }
            }
	  ptr += mlen;
	  length -= mlen;
	}
      else break;
    }
}
    
/**********************************************************************************
 *
 * send a message to one or more EZWGL applications.
 */
void EZ_SendMessage(mtype, message, length, mId, recipient, replyId, isA)
     Atom   mtype;
     char   *message;
     int    length, replyId, mId, isA;
     Window recipient;
{
  EZ_ApplRoster  *roster;
  Window         window;
  char           *p, *q, *msg;
  int           (*OldErrorHandler)();
  Atom  rclass, rinstance;

  msg = (char *)my_malloc((length+EZ_MSG_HEADER_LENGTH+1)*sizeof(char), _C_MESSAGE_);
  if(msg == NULL) EZ_OutOfMemory("EZ_BroadcastMessage");
  sprintf(msg, "%lx %lx %lx %lx %lx %lx %lx %lx",
	  (unsigned long)(length+EZ_MSG_HEADER_LENGTH),
	  (unsigned long)mtype,
	  (unsigned long)mId,
	  (unsigned long)EZ_DummyWindow,
	  (unsigned long)EZ_ClassAName,
	  (unsigned long)EZ_InstanceAName,
	  (unsigned long)replyId,
	  (unsigned long)isA);
  msg[EZ_MSG_HEADER_LENGTH] = 0;
  if(message && length > 0)
    {
      memcpy( (void *)(msg+EZ_MSG_HEADER_LENGTH), (void *)message, length);
      msg[length+EZ_MSG_HEADER_LENGTH] = 0;
    }
  
  roster = EZ_OpenEZWGLRoster(0);  /* grab the server removed 5-19-97 */
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      
      if(sscanf(q,"%lx %lx %lx",&window, &rclass, &rinstance) == 3)
	{
	  int doit = 0;

	  if(recipient == None) doit = 1; /* broadcast */
	  else if(recipient == window) doit = 1;
	  if(doit &&  EZ_VerifyExistence(window))
	    {
	      EZ_XErrorCode = 0;
	      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
	      XChangeProperty(EZ_Display,
			      (Window)window,
			      EZ_MESSAGE_ATOM,
			      XA_STRING, 8,
			      PropModeAppend,
			      (unsigned char *)msg,
			      length+EZ_MSG_HEADER_LENGTH
			      );
	      XSync(EZ_Display, False);
	      XSetErrorHandler(OldErrorHandler);  
	      if(recipient != None) break;
	    }
	}
    }
  EZ_CloseEZWGLRoster(roster);
  XFlush(EZ_Display);
  (void) my_free((char *)msg);
}
/***************************************************************************************
 *
 * reply a message
 */
void EZ_ReplyMessage(msg, reply, len)
     EZ_Message *msg;
     char *reply;
     int len;
{
  EZ_SendMessage(msg->messageType,
		 reply,
		 len,
		 (msg->replyId),
		 msg->sender,
		 0, 1);
}

void EZ_ReplyMessageR(msg, reply, len, replyId)
     EZ_Message *msg;
     char *reply;
     int len, replyId;
{
  EZ_SendMessage(msg->messageType,
		 reply,
		 len,
		 (msg->replyId),
		 msg->sender,
		 replyId, 1);
}

void  EZ_BroadcastMessage(mtype, message, len, mId, replyId)
     Atom mtype;
     char *message;
     int len, mId, replyId;
{
  EZ_SendMessage(mtype,
		 message,
		 len,
		 mId,
		 None,
		 replyId,
		 0);
}
/***************************************************************************************/
#undef _EZ_MESSAGE_C_
