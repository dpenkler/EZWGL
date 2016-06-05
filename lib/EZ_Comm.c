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

/* #define PRINT_MESSAGE  */
/*****************************************************************
 ***                                                           ***
 ***          Communication stuff                              ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_COMM_C_
#include "EZ_Widget.h"

/*****************************************************************
 *
 *     Function implemented in this file.
 */
void        *EZ_OpenEZWGLRoster MY_ANSIARGS((int lock));
void         EZ_CloseEZWGLRoster MY_ANSIARGS((EZ_ApplRoster *roster));
void         EZ_RegisterApplication MY_ANSIARGS((void));
void         EZ_UpdateRoster  MY_ANSIARGS((EZ_ApplRoster *roster));
int          EZ_WindowExist  MY_ANSIARGS((Window window));
int          EZ_SendEmbedingMessage  MY_ANSIARGS((int type,
						  Window fmdummy, Window fmwin,
						  unsigned long fmwidget,
						  Window todummy, Window towin,
						  unsigned long towidget,
						  unsigned long id,
						  int x, int y, int w, int h, 
						  int aux1, int aux2));
						  
void         EZ_HandlePropertyNotify MY_ANSIARGS((XEvent *event));
void         EZ_GenerateEmbedingRequest MY_ANSIARGS((EZ_Widget *widget,
						     int x, int y, int w, int h));
EZ_Widget    *EZ_SearchForEmbder MY_ANSIARGS((Window hint, int x, int y, int w, int h,
					      int *xx, int *yy));
EZ_Widget    *EZ_SetupAnEmbederIn MY_ANSIARGS((EZ_Widget *widget,
					       Window ecwin, Window  ewin,
					       unsigned long ewidget,
					       unsigned long timeS,
					       int x, int y, int w, int h ));
void         EZ_GrowAllocatedBuffer MY_ANSIARGS((char **buf, int *clength, 
						 int newlen, int unit));
void         EZ_HandleEmbedingMessages MY_ANSIARGS((char *data, 
						    unsigned long length));

void         EZ_DecodeEmbedingMessage MY_ANSIARGS((char *data, 
						   EZ_EmbedingMessage *msg));
void         EZ_EncodeEmbedingMessage MY_ANSIARGS((char *data, 
						   EZ_EmbedingMessage *msg));

unsigned long EZ_VerifyExistence  MY_ANSIARGS((Window window));
unsigned long EZ_WindowIsDnDTarget  MY_ANSIARGS((Window window));
unsigned long EZ_WindowIsDnDSrc  MY_ANSIARGS((Window window));

/**********************************************************/
static void EmbederTimerCallback MY_ANSIARGS((EZ_Timer *timer, void *data));

/**********************************************************
 *
 * Get all current EZWGL applications on EZ_Display.
 */
void *EZ_OpenEZWGLRoster(lock)
     int lock;
{
  EZ_ApplRoster *applRoster;
  Atom          aType;
  int           aFormat, ans;
  unsigned long bytesAfter;

  applRoster = (EZ_ApplRoster *)my_malloc(sizeof(EZ_ApplRoster), _ROSTER_);
  if(!applRoster) EZ_OutOfMemory("EZ_OpenEZWGLRoster");
  
  applRoster->grabServer = (char )lock;
  applRoster->needReset = 0;
  applRoster->needXFree = 1;
  applRoster->data = NULL;
  
  if(lock) EZ_GrabServer();
  
  /* get the EZ_APPLICATION_ATOM from the root window */
  ans = XGetWindowProperty(EZ_Display,
			   RootWindow(EZ_Display,EZ_ScreenNum),
			   EZ_APPLICATION_ATOM,
			   0, EZ_XServerDataReqSize,
			   False,
			   XA_STRING,
			   &aType,
			   &aFormat,
			   &(applRoster->length),
			   &bytesAfter,
			   (unsigned char **)&(applRoster->data));

  if(aType == None) 
    {
      applRoster->length = 0;
      applRoster->data = NULL;
    }
  else if((ans != Success) || (aFormat != 8) || (aType != XA_STRING))
    {
      /* property is trashed, reset applRoster and delete this prop */
      if(applRoster->data) 
	{
	  XFree(applRoster->data);
	  applRoster->data = NULL;
	  applRoster->length = 0;
	}
      XDeleteProperty(EZ_Display,
		      RootWindow(EZ_Display,EZ_ScreenNum),
		      EZ_APPLICATION_ATOM);
      XSync(EZ_Display, False);
    }
  return(applRoster);
}
    
/*************************************************************
 *
 * Reset the EZWGL Application Roster.
 */
void EZ_CloseEZWGLRoster(roster)
     EZ_ApplRoster *roster;
{

  if(roster->needReset)
    {
      if(roster->grabServer == 0)
	{
	  roster->grabServer = 1;
	  EZ_GrabServer();
	}
      XChangeProperty(EZ_Display,
		      RootWindow(EZ_Display,EZ_ScreenNum),
		      EZ_APPLICATION_ATOM,
		      XA_STRING, 8,
		      PropModeReplace,
		      (unsigned char *)roster->data,
		      (int) roster->length);
    }
  if(roster->grabServer) {EZ_UngrabServer();}
  XSync(EZ_Display, False);

  if(roster->data != NULL)
    {
      if(roster->needXFree) XFree(roster->data);
      else (void)my_free(roster->data);
    }
  (void)my_free(roster);
}
/****************************************************************/
Window *EZ_GetAllEZWGLClients()
{
  EZ_ApplRoster *roster = EZ_OpenEZWGLRoster(0);
  Window window, windows[100], *tt = NULL;
  int i, cnt = 0;
  char *p, *q;

  for(p = roster->data; cnt < 100 && (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      if(sscanf(q,"%lx",&window) == 1)
        { if(EZ_VerifyExistence(window)) windows[cnt++] = window; }
    }
  EZ_CloseEZWGLRoster(roster);
  if(cnt) 
    {
      tt = (Window *)malloc((cnt+1)*sizeof(Window));
      for(i = 0; i <cnt; i++) { tt[i] = windows[i];}
      tt[i] = None;
    }
  return(tt);
}

/*************************************************************
 *
 * Register an EZWGL application to roster.
 * Entries are in the format
 * window name \0 window name \0 ... 
 */

void EZ_RegisterApplication()
{
  char  str[256];
  char  *data;
  int   tlength;
  EZ_ApplRoster *roster;     

  /* First mark myself as an EZWGL application. */
  sprintf(str, "EZ");
  XChangeProperty(EZ_Display,
		  EZ_DummyWindow,
		  EZ_APPLICATION_ATOM,
		  XA_STRING, 8,
		  PropModeReplace,
		  (unsigned char *)str,
		  4);
  /* do this twice in order to stamp a server time to the property */
  {
    XEvent xevent;
    while(1)
      {
	XWindowEvent(EZ_Display, EZ_DummyWindow,PropertyChangeMask, &xevent);
	if(xevent.type == PropertyNotify) break;
      }
    if( (EZ_ApplicationSetupTime =  (unsigned long)(xevent.xproperty.time)) == 0)
      EZ_ApplicationSetupTime = 1;
    sprintf(str, "EZWGL %lx", EZ_ApplicationSetupTime);    
    XChangeProperty(EZ_Display,
		    EZ_DummyWindow,
		    EZ_APPLICATION_ATOM,
		    XA_STRING, 8,
		    PropModeReplace,
		    (unsigned char *)str,
		    (int) strlen(str));    
  }
  
  /* register to roster */
  roster = EZ_OpenEZWGLRoster(1);
  EZ_UpdateRoster(roster); /* clean up entries that are no longer active */
  
  sprintf(str,"%lx %lx %lx",EZ_DummyWindow, (unsigned long)EZ_ClassAName,
	  (unsigned long)EZ_InstanceAName);
  tlength = strlen(str) + 1;

  data = (char *)my_malloc((unsigned) (tlength + roster->length)* sizeof(char), _REGIS_DATA_);
  
  if(!data) EZ_OutOfMemory("EZ_RegisterApplication");
  strcpy(data, str);

  if(roster->data != NULL)
    {
      memcpy( (void *)(data + tlength), (void *)roster->data,roster->length);
      if(roster->needXFree) XFree(roster->data);
      else (void)my_free(roster->data);
    }
  roster->needXFree = 0;
  roster->needReset = 1;
  roster->length += tlength;
  roster->data = data;
  EZ_CloseEZWGLRoster(roster);  
}
/*************************************************************
 *
 * Verify whether or not an EZWGL application still exists
 */
unsigned long EZ_VerifyExistence(commWindow)
     Window commWindow;
{
  int    (*OldErrorHandler)();
  Atom          aType;
  int           aFormat = 0, ans;
  char          *data = NULL;
  unsigned long bytesAfter, length;
  unsigned long  ret = 0;

  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  ans = XGetWindowProperty(EZ_Display,
			   commWindow,
			   EZ_APPLICATION_ATOM,
			   0, EZ_XServerDataReqSize,
			   False,
			   XA_STRING,
			   &aType,
			   &aFormat,
			   &length,
			   &bytesAfter,
			   (unsigned char **)&data);
  XSetErrorHandler(OldErrorHandler);  

  if((EZ_XErrorCode == 0) && (ans == Success) &&
     (aFormat == 8) && (aType == XA_STRING))
    {
      if(data[0] == 'E' && data[1] == 'Z') 
	sscanf(data+5, "%lx", &ret);
    }
  if(data != NULL) XFree(data);
  return(ret);
}

/************************************************************
 *
 *  Check whether window is a DnDTarget.
 */
unsigned long EZ_WindowIsDnDTarget(dndWindow)
     Window dndWindow;
{
  int    (*OldErrorHandler)();
  Atom          aType;
  int           aFormat = 0, ans;
  unsigned long ret = 0L;
  char          *data = NULL;
  unsigned long bytesAfter, length;
  
  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  ans = XGetWindowProperty(EZ_Display,
			   dndWindow,
			   EZ_DND_TWIN_ATOM,
			   0, EZ_XServerDataReqSize,
			   False,
			   XA_STRING,
			   &aType,
			   &aFormat,
			   &length,
			   &bytesAfter,
			   (unsigned char **)&data);
  XSetErrorHandler(OldErrorHandler);  
  if((EZ_XErrorCode == 0) && (ans == Success) &&
     (aFormat == 8) && (aType == XA_STRING))
    {
      if(!strncmp(data, "EZ_DND_TWIN",9)) 
	sscanf(data+12, "%lx", &ret);
    }
  if(data != NULL) XFree(data);

  return(ret);
}

unsigned long EZ_WindowIsDnDSrc(dndWindow)
     Window dndWindow;
{
  int    (*OldErrorHandler)();
  Atom          aType;
  int           aFormat = 0, ans;
  unsigned long ret = 0L;
  char          *data = NULL;
  unsigned long bytesAfter, length;

  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  ans = XGetWindowProperty(EZ_Display,
			   dndWindow,
			   EZ_DND_SWIN_ATOM,
			   0, EZ_XServerDataReqSize,
			   False,
			   XA_STRING,
			   &aType,
			   &aFormat,
			   &length,
			   &bytesAfter,
			   (unsigned char **)&data);
  XSetErrorHandler(OldErrorHandler);  
  if((EZ_XErrorCode == 0) && (ans == Success) &&
     (aFormat == 8) && (aType == XA_STRING))
    {
      if(!strncmp(data, "EZ_DND_SWIN",9)) 
	sscanf(data+12, "%lx", &ret);
    }
  if(data != NULL) XFree(data);
  return(ret);
}

/**************************************************************
 *
 *  Verify the existence of a window.
 */
int  EZ_WindowExist(window)
     Window window;
{
  int ans = 0;
  Window root_return;
  Window parent_return;
  unsigned int nchildren_return;
  Window *children_return = (Window *)NULL;
  int    (*OldErrorHandler)();

  if(window)
    {
      EZ_XErrorCode = 0;
      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
      ans = XQueryTree(EZ_Display,window, &root_return, &parent_return,
		       &children_return, &nchildren_return);
      XSetErrorHandler(OldErrorHandler);  
      if(ans != 0 && EZ_XErrorCode == 0)
	{
	  if(children_return != (Window *)NULL) XFree(children_return);
	  return(1);
	}
    }
  return(0);
}
/**************************************************************
 *
 * Go through the roster and verify each registered application
 * remove the registration for applications that do not exist
 * anymore.
 */
void EZ_UpdateRoster(roster)
     EZ_ApplRoster *roster;     
{
  Window win, me;
  char *p, *q, *r;
  int i;

  me = EZ_DummyWindow;
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      r = p;
      
      while( *p != 0) p++; /* forward to the next entry */
      p++; 

      if(sscanf(r,"%lx",&win) == 1)
	{
	  if(win == me || EZ_VerifyExistence(win) == 0 || EZ_WindowExist(win) == 0) /* application is gone */
	    {
	      i = roster->length - (p - roster->data);
	      roster->length -= p-q;
	      roster->needReset = 1;
	      if(i > 0)
		{
		  int j;
		  char *tdes,*tsrc;
		  for(j = 0, tdes = q, tsrc = p; j < i; j++) *tdes++ = *tsrc++;
		  /* memmove((void *)q, (void *)p, (size_t)i); */
		  p = q;
		}
	    }
	}
      else /* something bad */
	(void) fprintf(stderr, "Application Roster: bad entry\n");
    }
}
/********************************************************
 *
 * Send a message to a client.
 */
int EZ_SendEmbedingMessage(type, fmdummy, fmwin, fwidget, todummy, towin,twidget,
			   id, x,y,w,h, aux1, aux2)
     Window todummy, fmdummy, towin, fmwin;
     unsigned long fwidget, twidget;
     unsigned long id;
     int type;
     int x,y,w,h, aux1, aux2;
{
  EZ_EmbedingMessage theMsg;     
  char message[256];           /* this is big enough !*/
  int  (*OldErrorHandler)();

  theMsg.type = type;
  theMsg.fmCommWin =  (unsigned long)fmdummy;
  theMsg.fmWin =  (unsigned long)fmwin;
  theMsg.fmWidget = fwidget;
  theMsg.toCommWin =  (unsigned long)todummy;
  theMsg.toWin =  (unsigned long)towin;
  theMsg.toWidget = twidget;
  theMsg.id = id;
  theMsg.x = x;  theMsg.y = y;   theMsg.w = w;   theMsg.h = h;
  theMsg.a1 = aux1;   theMsg.a2 = aux2;

  EZ_EncodeEmbedingMessage(message, &theMsg);
  
  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  /* EZ_GrabServer();*/
  XChangeProperty(EZ_Display,
		  todummy,
		  EZ_EMBEDING_MESSAGE_ATOM,
		  XA_STRING, 8,
		  PropModeAppend,
		  (unsigned char *)message,
		  theMsg.length);  
  /* EZ_UngrabServer();*/
  XSync(EZ_Display, False);
  XSetErrorHandler(OldErrorHandler);   
  return(EZ_XErrorCode == 0);
}

/********************************************************/ 
int EZ_GetWindowProperty(dummyWin, prop_type, data_ret, length_ret, remove)
     Window dummyWin;
     Atom prop_type;
     char **data_ret;
     unsigned long *length_ret;
     int   remove;
{
  int    (*OldErrorHandler)();
  Atom          aType;
  int           aFormat = 0, ans;
  unsigned long bytesAfter, totalBytes = 0;
  unsigned long length = 0;
  char          *property = NULL;
  int           allocatedLength = 0;
  char          *data = NULL;
  int           done = 0;
  
  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  /* EZ_GrabServer(); */
  while(!done)
    {
      ans = XGetWindowProperty(EZ_Display,
			       dummyWin,
			       prop_type,
			       totalBytes/4,       /* 32-bit offset */
			       EZ_XServerDataReqSize,
			       remove,
			       XA_STRING,
			       &aType,
			       &aFormat,
			       &length,
			       &bytesAfter,
			       (unsigned char **)&data);
      if(EZ_XErrorCode == 0 && (ans == Success))
	{
	  if((aFormat == 8) && (aType == XA_STRING))
	    {
	      int itmp = length + totalBytes + 8;
	      char *ptr;
	      EZ_GrowAllocatedBuffer(&property, &allocatedLength, itmp, EZ_XServerDataReqSize);
	      ptr = property + totalBytes;
	      memcpy( (void *)(ptr), (void *)data, length);	  
	      totalBytes += length;
	      XFree(data); data = NULL;
	    }
	  else {if(data) XFree(data), data = NULL; }
	  if(bytesAfter <= 0) done = 1;
	}
      else done = 1;
    }
  /* EZ_UngrabServer();*/
  XSetErrorHandler(OldErrorHandler); 
  *data_ret = property;
  *length_ret = totalBytes;
  if(totalBytes > 0) property[totalBytes] = '\0';
  
  return( (totalBytes > 0));
}

/***********************************************************************************
 *
 *  Handle PropertyNotify events on the communication window.
 */

void EZ_HandlePropertyNotify(event)
     XEvent *event;
{
  char          *data = NULL;
  unsigned long length;
  Window dummyWin = EZ_DummyWindow;

  if(EZ_GetWindowProperty(dummyWin, EZ_DND_MESSAGE_ATOM, &data, &length, True))
    {
      EZ_HandleDnDMessage(data, length);
    }
  else if(EZ_GetWindowProperty(dummyWin, EZ_MESSAGE_ATOM, &data, &length, True))
    {
      EZ_HandleGeneralMessage(data, length);
    }
  else if(EZ_GetWindowProperty(dummyWin, EZ_EMBEDING_MESSAGE_ATOM, &data, &length, True))
    {
      EZ_HandleEmbedingMessages(data,length);
    }
  if(data) (void)my_free(data);
}
/*********************************************************************************/
void EZ_HandleEmbedingMessages(data,length)
     char *data;
     unsigned long length;
{
  EZ_EmbedingMessage theMsg;
  EZ_Widget *widget;
  Window fdwin, fwin, mdwin, mwin;
  unsigned long fwidget, mwidget;
  unsigned long id;
  int  type;
  int  x,y,w,h,aux1,aux2;
  char *p, *q;
  
  for(p = data; (p - data) < length;)
    {
      q = p; 
      
      EZ_DecodeEmbedingMessage(q, &theMsg);

      if(theMsg.length == 0) break;
      p = p + theMsg.length;
      
      if(theMsg.type)
	{
	  type = theMsg.type;
	  fdwin = (Window) theMsg.fmCommWin;
	  fwin  = (Window) theMsg.fmWin;
	  fwidget = (unsigned long) theMsg.fmWidget;
	  mdwin = (Window) theMsg.toCommWin;
	  mwin  = (Window) theMsg.toWin;
	  mwidget = (unsigned long) theMsg.toWidget;
	  id = (unsigned long) theMsg.id;
	  
	  x = theMsg.x;
	  y = theMsg.y;
	  w = theMsg.w;
	  h = theMsg.h;
	  aux1 = theMsg.a1;
	  aux2 = theMsg.a2;

	  switch(type)
	    {
	    case EZ_EMBEDING_REQUEST_S:  /* request for embeding */
	      /* verify that fdwin and fwin are valid; look to see if there is
	       * a widget capable of embdeding win. Send a message back if yes.
	       */
	      {
		int xx,yy, xoff, yoff;
		unsigned long pid = EZ_VerifyExistence(fdwin);
		if(id == pid && EZ_WindowExist(fwin) &&
		   (widget = EZ_SearchForEmbder(mwin, x,y,w,h,&xx,&yy)) != (EZ_Widget *)NULL)
		  {
		    EZ_Widget *tmp = EZ_SetupAnEmbederIn(widget, /* create an embeder */
							 fdwin, fwin, fwidget,pid,
							 xx,yy,w,h);
		    xoff = EZ_WidgetBorderWidth(tmp) + EZ_WidgetPadX(tmp);
		    yoff = EZ_WidgetBorderWidth(tmp) + EZ_WidgetPadY(tmp);
		    EZ_SendEmbedingMessage(EZ_EMBEDING_REPLY_C,
					   EZ_DummyWindow, EZ_WidgetWindow(tmp),
					   (unsigned long)tmp, 
					   fdwin, fwin, fwidget, 
					   EZ_ApplicationSetupTime,
					   xoff,yoff,w,h, 0, 0);
		  }
	      }
	    break;
	    case EZ_EMBEDING_REPLY_C: /* do embeding */
	      if(id == EZ_VerifyExistence(fdwin) &&
		 EZ_WindowExist(fwin) && mdwin == EZ_DummyWindow && 
		 (widget = EZ_LookupWidgetFromAllHTUL(mwidget)) != (EZ_Widget *)NULL)
		{
		  if(EZ_GetWidgetEmbededFlag(widget) == 0)
		    {/* do it only if its not embeded yet */
		      XReparentWindow(EZ_Display,EZ_WidgetWindow(widget),
				      fwin, x,y);
		      EZ_SetWidgetEmbededFlag(widget);
		      EZ_WidgetDnDApplName(widget) = fdwin;
		      EZ_WidgetDnDParent(widget) = fwin;
		      EZ_WidgetOriginX(widget) = x;
		      EZ_WidgetOriginY(widget) = y;
		      EZ_DisplayWidget(widget);
		      EZ_SendEmbedingMessage(EZ_EMBEDING_CONFIRM_S,
					     EZ_DummyWindow, EZ_WidgetWindow(widget), 
					     (unsigned long)widget,
					     fdwin, fwin, fwidget, 
					     EZ_ApplicationSetupTime,
					     x,y,
					     EZ_WidgetWidth(widget),
					     EZ_WidgetHeight(widget), 0, 0);
		    }
		}
	      break;		  
	    case EZ_EMBEDING_CONFIRM_S:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    /* an embeding is confirmed. display the embeder */
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp)
		      {
			EZ_ClearWidgetNonActiveFlag(tmp);
			EZ_EmbederConfirmed(tmp) = 1;
			EZ_EmbederCommuWin(tmp) = fdwin;
			EZ_EmbederWin(tmp) = fwin;
			EZ_EmbederWidgetAddr(tmp) = fwidget; 
			EZ_EmbederX(tmp) = x;
			EZ_EmbederY(tmp) = y;
			EZ_EmbederWidth(tmp) = w;
			EZ_EmbederHeight(tmp) = h;
			
			if(EZ_WidgetType(tmp) == EZ_WIDGET_EXECUTOR)
			  {
			    /* for executor, the embeded application sends a confirmation
			     * message when it's main window is created. This is the only
			     * confirmation. We need to redisplay the executor b/c only
			     * after this event the ececutor knows the geometry of its client.
			     */
			    EZ_ExecutorMinW(tmp) = aux1;
			    EZ_ExecutorMinH(tmp) = aux2;
			    EZ_EmbederId(tmp) = EZ_VerifyExistence(fdwin); /* remember the id */
			    EZ_ClearWidgetSizeComputedFlag(tmp);  
			    EZ_ExecutorMayBeDirty(tmp) = 1;         /* !!! */
			  }
			EZ_DisplayWidget(tmp);
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_UNMAPPED_S:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    /* an embeded window is unmapped */
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp && EZ_WidgetType(tmp) == EZ_WIDGET_EMBEDER &&
		       EZ_GetWidgetNonActiveFlag(tmp) == 0)
		      {
			EZ_SetWidgetNonActiveFlag(tmp);
			EZ_HideWidget(tmp);
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_REMAPPED_S:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    /* an embeded window has just been re-mapped */
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp && EZ_GetWidgetNonActiveFlag(tmp))
		      {
			EZ_ClearWidgetNonActiveFlag(tmp);
			EZ_DisplayWidget(tmp);
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_CLIENT_DIED_S:
	      {
		/* embeded application has been terminated. Destroy
		 * the embeder
		 */
		EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		if(tmp)
		  {
		    if(EZ_WidgetType(tmp) == EZ_WIDGET_EMBEDER)
		      {
			if(EZ_EmbederId(tmp) == id)
			  {
			    EZ_EmbederConfirmed(tmp) = 0;
			    EZ_EmbederCommuWin(tmp) = (Window)NULL;
			    EZ_EmbederWin(tmp) = (Window)NULL;
			    EZ_EmbederWidgetAddr(tmp) = 0;
			    EZ_DestroyWidget(tmp);
			  }
		      }
		    else if(EZ_WidgetType(tmp) == EZ_WIDGET_EXECUTOR)
		      {
			if(EZ_EmbederId(tmp) == id)
			  {
			    EZ_EmbederId(tmp) = 0;
			    EZ_ExecutorMinW(tmp) = 1;
			    EZ_ExecutorMinH(tmp) = 1;
			    EZ_ClearWidgetSizeComputedFlag(tmp);  
			    EZ_DisplayWidget(tmp);
			  }
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_DESTROY_YOUSELF_C:
	      {
		/* embeder has been destroyed or in the process of destruction.
		 * Exit the application. To do: open an dialogue to request for
		 * confirmation. 
		 */
		EZ_Widget *tmp = EZ_LookupWidgetFromAllHTUL(mwidget);
		if(tmp && id == EZ_ApplicationSetupTime) 
		  {
		    XSync(EZ_Display, False);
		    EZ_DestroyWidget(tmp);
		    EZ_ShutdownA();
		    if(data != NULL) my_free(data);
		    XCloseDisplay(EZ_Display);
		    exit(EZ_EXIT_OK);
		  }
	      }
	    break;
	    case EZ_EMBEDING_MANAGE_YOUSELF_C:
	      {
		/* embeder has been moved out of its parent's window. The application
		 * is no longer considered to be embeded. 
		 */
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    EZ_Widget *tmp = EZ_LookupWidgetFromAllHTUL(mwidget);
		    if(tmp)
		      {
			Window pwin = EZ_WidgetDnDParent(tmp);
			EZ_WidgetOriginX(tmp) = x;
			EZ_WidgetOriginY(tmp) = y;
			EZ_SetWMHintsAndSizeHints(tmp, 1);
			XReparentWindow(EZ_Display,
					EZ_WidgetWindow(tmp),
					RootWindow(EZ_Display, EZ_ScreenNum),
					x,y);
			if(EZ_WindowExist(pwin)) XDestroyWindow(EZ_Display, pwin);
			EZ_ClearWidgetEmbededFlag(tmp);
			/* EZ_WidgetDnDApplName(tmp) = (Window)NULL; */
			EZ_WidgetDnDApplName(tmp) = (Window)fwidget; /* 1-19-97 */
			EZ_WidgetDnDParent(tmp) = (Window)NULL;
			EZ_SetWidgetXYSetFlag(tmp);
			EZ_DisplayWidget(tmp);
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_WINDOW_RESIZED_S: /* embeded appl is resized */
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp)
		      {
			if(EZ_WidgetType(tmp) == EZ_WIDGET_EXECUTOR)
			  {
			    if(EZ_ExecutorMinW(tmp) != aux1 ||
			       EZ_ExecutorMinH(tmp) != aux2 ||
			       EZ_EmbederWidth(tmp) != w    ||
			       EZ_EmbederHeight(tmp) != h)
			      {
				EZ_ExecutorMinW(tmp) = aux1;
				EZ_ExecutorMinH(tmp) = aux2;
				EZ_EmbederWidth(tmp) = w;
				EZ_EmbederHeight(tmp) = h; 
				EZ_ExecutorMayBeDirty(tmp) = 1;
				EZ_ClearWidgetSizeComputedFlag(tmp);
				EZ_DisplayWidget(tmp);
			      }
			  }
			else
			  {
			    if( EZ_EmbederWidth(tmp) != w ||
				EZ_EmbederHeight(tmp) != h)
			      {
				EZ_EmbederWidth(tmp) = w;
				EZ_EmbederHeight(tmp) = h;			   
				EZ_ClearWidgetSizeComputedFlag(tmp);
				EZ_DisplayWidget(tmp);
			      }
			  }
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_MOVE_WINDOW_C:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp)
		      {
			if( EZ_WidgetOriginX(tmp) != x ||
			    EZ_WidgetOriginY(tmp) != y)
			  {
			    EZ_WidgetMaped(tmp) = 4;
			    EZ_WidgetOriginX(tmp) = x;
			    EZ_WidgetOriginY(tmp) = y;
			    EZ_SetWidgetXYSetFlag(tmp);
			    EZ_ClearWidgetSizeComputedFlag(tmp);
			    EZ_DisplayWidget(tmp);
			  }
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_RESIZE_WINDOW_C:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp)
		      {
			if(EZ_WidgetWidth(tmp) != w ||
			   EZ_WidgetHeight(tmp) != h)
			  {
			    EZ_WidgetWidth(tmp) = w;
			    EZ_WidgetHeight(tmp) = h;
			    EZ_WidgetWidthHint(tmp) = w;
			    EZ_WidgetHeightHint(tmp) = h;
			    EZ_SetWidgetSizeSetFlag(tmp);
			    EZ_ClearWidgetSizeComputedFlag(tmp);
			    EZ_WidgetMaped(tmp) = 4;
			    EZ_DisplayWidget(tmp);			  
			  }
		      }
		  }
	      }
	    break;
	    case EZ_EMBEDING_MOVE_RESIZE_WINDOW_C:
	      {
		if(id == EZ_VerifyExistence(fdwin))
		  {
		    EZ_Widget *tmp = EZ_LookupWidgetFromMappedHT(mwin);
		    if(tmp)
		      {
			if(EZ_WidgetOriginX(tmp) != x ||
			   EZ_WidgetOriginY(tmp) != y ||
			   EZ_WidgetWidth(tmp) != w   ||
			   EZ_WidgetHeight(tmp) != h) 
			  {
			    EZ_WidgetOriginX(tmp) = x;
			    EZ_WidgetOriginY(tmp) = y;
			    EZ_WidgetWidth(tmp) = w;
			    EZ_WidgetHeight(tmp) = h;
			    EZ_WidgetWidthHint(tmp) = w;
			    EZ_WidgetHeightHint(tmp) = h;
			    EZ_SetWidgetXYSetFlag(tmp);
			    EZ_SetWidgetSizeSetFlag(tmp);
			    EZ_ClearWidgetSizeComputedFlag(tmp);
			    EZ_WidgetMaped(tmp) = 4;
			    EZ_DisplayWidget(tmp);
			  }
		      }
		  }
	      }
	    break;
	    default:
	      break;
	    }
	}
    }
}
/******************************************************
 *
 * A toplevel widget has been moved.  Send message to
 * all EZWGL applications to look for an embeder.
 */
void EZ_GenerateEmbedingRequest(widget, x,y,w,h)
     EZ_Widget *widget;
     int x,y,w,h;
{
  EZ_EmbedingMessage theMsg;
  Window win = EZ_WidgetWindow(widget);
  char message[256];
  char *p, *q;
  int length;
  EZ_ApplRoster *roster;
  int    (*OldErrorHandler)();
  
  theMsg.type = EZ_EMBEDING_REQUEST_S;
  theMsg.id = EZ_ApplicationSetupTime;
  theMsg.fmCommWin =  (unsigned long)EZ_DummyWindow;
  theMsg.fmWin =  (unsigned long)win;
  theMsg.fmWidget = (unsigned long)widget;
  theMsg.toCommWin = (unsigned long)EZ_WidgetDnDApplName(widget);   /* receiver, hint */
  theMsg.toWin =  0L;
  theMsg.toWidget = 0L;
  theMsg.x = x;  theMsg.y = y;   theMsg.w = w;   theMsg.h = h;
  theMsg.a1 = 0;   theMsg.a2 = 0;

  EZ_WidgetDnDApplName(widget) = (Window )NULL;
  
  /* generate message */
  EZ_EncodeEmbedingMessage(message, &theMsg);
  length = theMsg.length;
  
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler); 
  roster = EZ_OpenEZWGLRoster(0);
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      
      if(sscanf(q,"%lx",&win) == 1)
	{
	  if(EZ_VerifyExistence(win))
	    {
	      XChangeProperty(EZ_Display,
			      (Window) win,
			      EZ_EMBEDING_MESSAGE_ATOM,
			      XA_STRING, 8,
			      PropModeAppend,
			      (unsigned char *)message,
			      length);	
	    }
	}
    }
  EZ_CloseEZWGLRoster(roster);
  XSync(EZ_Display, False);
  XSetErrorHandler(OldErrorHandler);  
}


/*******************************************************
 *
 * Embeder timer callback.
 *
 */
static void EmbederTimerCallback(timer, data)
     EZ_Timer *timer;
     void     *data;
{
  if(timer)
    {
      EZ_Widget *widget = (EZ_Widget *)data;
      int quit = 0;
      if(EZ_LookupWidgetFromAllHT(widget))
	{
	  Window commWin, win;
	  commWin = EZ_EmbederCommuWin(widget);
	  win = EZ_EmbederWin(widget);
      
	  /* not confirmed or confirmed but somehow the       
	   * embeded application has been aborted 
	   */
	  if( EZ_EmbederConfirmed(widget) == -1 ) quit = 1;
	  else 
	    {
	      unsigned long id = EZ_VerifyExistence(commWin);
	      if(id != EZ_EmbederId(widget) || EZ_WindowExist(win) == 0) quit = 1;
	    }
	  if(quit)
	    {
	      EZ_EmbederConfirmed(widget) = 0;
	      EZ_EmbederWin(widget) = (Window)NULL;
	      EZ_EmbederCommuWin(widget) = (Window)NULL;
	      EZ_EmbederWidgetAddr(widget) = 0;
	      EZ_DestroyWidget(widget);
	      EZ_CancelTimer(timer);
	    }
	}
      /* ptr does'nt exist or is not an embeder, release timer */
      else  EZ_CancelTimer(timer); 
    }
}
/*******************************************************
 *
 * An external application is about to be embeded in widget.
 * Create a embeder widget. Create a window for it. 
 * Mark the widget non-active. It will be activated when it
 * receive a EZ_EMBED_CONFIRM message.
 * Also create a timer to check constantly whether the embeder
 * has been confirmed. Destroy the embeder if it has not been
 * confirmed for 5 seconds.
 */
EZ_Widget *EZ_SetupAnEmbederIn(widget,ecwin, ewin, ewidget, timeS, x,y, w, h)
     EZ_Widget *widget;
     Window ecwin, ewin;
     unsigned long ewidget; 
     unsigned long timeS;
     int x,y,w,h;
{
  int ww, hh;
  EZ_Widget *tmp = EZ_CreateEmbeder(widget);
  EZ_GetAndUseWidgetResources(tmp);
  (void) EZ_CreateTimer(5,0,-1, (EZ_CallBack)EmbederTimerCallback, (void *)tmp, 0); 
  EZ_EmbederCommuWin(tmp) = ecwin;
  EZ_EmbederWin(tmp) = ewin;
  EZ_EmbederWidgetAddr(tmp) = ewidget;
  EZ_EmbederWidth(tmp) = w;
  EZ_EmbederHeight(tmp) = h;
  EZ_EmbederId(tmp) = timeS;
  EZ_WidgetOriginX(tmp) = x;
  EZ_WidgetOriginY(tmp) = y;
  EZ_ComputeWidgetWindowSize(tmp, &ww, &hh, NULL);
  EZ_CreateWindowForWidget(tmp, 1);
  EZ_SetWidgetNonActiveFlag(tmp);
  return(tmp);
}

/********************************************************/  
void EZ_GrowAllocatedBuffer(buffer, clength, newlength, unit)
     char **buffer;
     int  *clength, newlength, unit;
{
  if(*buffer == (char *)NULL) /* first time */
    {
      int tmp = (newlength+unit);
      *buffer = (char *)my_malloc( tmp*sizeof(char), _PROPERTY_DATA_);
      *clength = tmp;
    }
  else
    {
      if( (*clength) + 1 < newlength)
	{
	  int tmp = (newlength+unit);
	  *buffer = (char *)my_realloc(*buffer, tmp, _PROPERTY_DATA_);
	  *clength = tmp;
	}
    }
}

/********************************************************/  
extern EZ_MappedWidgetHTEntry  EZ_MappedWidgetsHashTable[];

EZ_Widget *EZ_SearchForEmbder(hintwin,x,y,w,h,xx,yy)
     Window hintwin;        /* try this window first. Later ... */
     int x,y,w,h,*xx,*yy;
{
  EZ_Widget *widget;
  EZ_Widget *wlist[64];
  int        xywh[64][4];
  int        nwidgets = 0, minW = 100000, minH = 100000;
  
  EZ_MappedWidgetHTEntry *htptr;
  Window win, tmpwin;
  int i, ox,oy, tx,ty, bw, width, height, WType;
  
  /* todo:   use hint */
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      htptr = EZ_MappedWidgetsHashTable[i].next;
      while( htptr)
	{
	  widget = htptr->widget; 
	  win = EZ_WidgetWindow(widget);
	  WType = EZ_WidgetType(widget);
	  if(WType != EZ_WIDGET_EMBEDER && EZ_GetWidgetEmbederFlag(widget) &&
	     EZ_WidgetMapped(widget) && EZ_WindowExist(win))
	    {
	      bw = EZ_WidgetBorderWidth(widget) + 1;
	      width = EZ_WidgetWidth(widget) - (bw + bw);
	      height = EZ_WidgetHeight(widget) - (bw + bw);
	      if(width > w && height > h)
		{
		  XTranslateCoordinates(EZ_Display, win,
					RootWindow(EZ_Display, EZ_ScreenNum),
					bw,bw, &ox,&oy, &tmpwin);
		  tx = x-ox; ty = y-oy;
		  if(ox < x && oy < y && width - tx > w  && height - ty > h) 
		    {
		      wlist[nwidgets] = widget;
		      xywh[nwidgets][0] = tx;     xywh[nwidgets][1] = ty;
		      xywh[nwidgets][2] = width;  xywh[nwidgets][3] = height;	      
		      if(minW > width)  minW= width;
		      if(minH > height) minH= height;
		      if( (++nwidgets) == 64) break;
		    }
		}
	    }
	  htptr = htptr->next;
	}
    }
  if(nwidgets > 0)
    {
      int i;
      for(i = 0; i < nwidgets; i++)
	if(xywh[i][2] == minW && xywh[i][3] == minH)
	  {
	    *xx = xywh[i][0];
	    *yy = xywh[i][1];	    
	    return(wlist[i]);
	  }
    }
  return(NULL);
}

/***************************************************************************
 *
 *  1-24-97:  Embeding message encoding/decoding. Separated from message
 *            handle. Will be easier later if we change the messaging scheme.
 */

void EZ_DecodeEmbedingMessage(data, msg_ret)
     char *data;
     EZ_EmbedingMessage *msg_ret;     
{
  char *q = data;

  if(sscanf(q, "%d %lx", &(msg_ret->type), &(msg_ret->length)) != 2)
    {
      msg_ret->type = 0;
      msg_ret->length = 0;
      return;
    }
  q =q + 20; /* the first 20 characters is reserved for property length and type */
  if(sscanf(q,"%lx %lx %lx %lx %lx %lx %lx %d %d %d %d %d %d",
	    &(msg_ret->fmCommWin),
	    &(msg_ret->fmWin),
	    &(msg_ret->fmWidget),
	    &(msg_ret->toCommWin),
	    &(msg_ret->toWin),
	    &(msg_ret->toWidget),
	    &(msg_ret->id),
	    &(msg_ret->x),
	    &(msg_ret->y),
	    &(msg_ret->w),
	    &(msg_ret->h),
	    &(msg_ret->a1),
	    &(msg_ret->a2)) != 13)
    msg_ret->type = 0;
}
/***************************************************************************************/
void EZ_EncodeEmbedingMessage(data, message)
     char *data;
     EZ_EmbedingMessage *message;
{
  char *ptr = data + 20;
  int length;

  sprintf(ptr, "%lx %lx %lx %lx %lx %lx %lx %d %d %d %d %d %d",
	  message->fmCommWin, message->fmWin, message->fmWidget,
	  message->toCommWin, message->toWin, message->toWidget, message->id,
	  message->x, message->y, message->w, message->h,
	  message->a1, message->a2);
  length = strlen(ptr) + 21;   /* append a \0 at the end */
  message->length = length;
  
  sprintf(data,"%d %lx ", /* preppend the length in front of the message */
	  message->type,
	  message->length);
}
/***************************************************************************************/
#undef _EZ_COMM_C_
