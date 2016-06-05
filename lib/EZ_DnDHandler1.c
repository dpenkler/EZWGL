/***********************************************************************
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
#ifdef XK_KP_F1
#define CASE_XK_KP_F1  /**/case/**/XK_KP_F1:/**/
#else
#define CASE_XK_KP_F1 /**/
#endif

#define EZ_DnDEventHandler1(_widget_,_event_) \
{\
  EZ_Widget *widget_ = (_widget_);\
  XEvent xevent;\
  Window ptWin = (_event_)->xbutton.window;\
  Window oldPtWin = ptWin;\
  Window widgetwin = ptWin;\
  int whichBtn = (_event_)->xbutton.button;\
\
  if(EZ_WidgetIsDnDSrc(widget_))\
    {\
       unsigned long serial = 0L;\
       unsigned int actionType = (_event_->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);\
       int newEvent = 0;\
       int done = 0;\
       int moved = 0;\
       int grabed = 0;\
       int dragCancelled = 0;\
       int helpRequested = 0;\
       int x = (_event_)->xbutton.x;\
       int y = (_event_)->xbutton.y;\
       int Sx,Sy,Ox,Oy,tx,ty,iww,ihh,iarea,oldx,oldy;\
       int wox,woy, snapshot = 0;\
\
       /* */ Ox = Oy = oldx = oldy = 0; iww = ihh = 1;\
       while(!done)\
	 {\
	   while(XPending(EZ_Display) && !done) \
	     {\
	       XNextEvent(EZ_Display, &xevent);\
	       if(EZ_FilterEvent(&xevent))\
	          EZ_InvokePrivateEventHandler(&xevent);\
	       newEvent = 1;\
	       if(xevent.type == MotionNotify) \
		 { \
		   moved = 1;\
		   serial = xevent.xmotion.serial; \
                    ptWin = xevent.xmotion.window; \
		   if(xevent.xmotion.window == widgetwin)\
		     {\
                      if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||\
			 xevent.xmotion.x >= EZ_WidgetWidth(widget_) ||  \
			 xevent.xmotion.y >= EZ_WidgetHeight(widget_))\
			ptWin = 0L;\
		     }\
		   if(helpRequested)\
		     {\
		       EZ_DnDSendCancelHelpMsg();\
		       EZ_DnDInfo.ptWin = (unsigned long)ptWin;\
		       helpRequested = 0;\
		     }\
		 }\
	       else if(xevent.type == ButtonRelease)\
		 {\
		   if(xevent.xbutton.button == whichBtn) \
		     {\
		       ptWin = xevent.xbutton.window; \
		       if(xevent.xbutton.window == widgetwin)\
			 {\
			    if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||\
			       xevent.xbutton.x >= EZ_WidgetWidth(widget_) ||  \
			       xevent.xbutton.y >= EZ_WidgetHeight(widget_))\
			      ptWin = 0L;\
			 }\
		       done = 1;\
		       break;\
		     }\
		 }\
	       else if(xevent.type == KeyPress)\
		 {\
		   KeySym            keysym;\
		   XComposeStatus    compose; \
		   char              tmpbuffer[4];\
		   int               count;\
		   count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);\
		   switch(keysym)\
		     {\
		     case XK_F1:  /* help */\
                     CASE_XK_KP_F1\
		       if(grabed && helpRequested == 0 && ptWin != 0)\
			 {\
			   EZ_DnDInfo.ptWin = (unsigned long)ptWin;\
			   EZ_DnDSendRequestHelpMsg();\
			   helpRequested = 1;\
			 }\
		       break;\
		     case XK_Escape:   /* cancel */\
		       {\
			 dragCancelled = 1;\
			 done = 1;\
		       }\
		       break;\
		     default:\
		       break;\
		     }\
		 }\
	       else if(xevent.type == Expose ||xevent.type==FocusIn || xevent.type ==FocusOut)\
		EZ_WidgetDispatchEvent(&xevent);\
			  \
	       /* if ptWin has changed, send a leave window message */\
	       if(oldPtWin != ptWin)\
		 {\
		   if(grabed && oldPtWin != 0)\
		     {\
		       unsigned long twin = EZ_DnDInfo.ptWin;\
		       EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);\
		       EZ_DnDSendLeaveWindowMsg();\
		       EZ_DnDInfo.ptWin = twin;\
		     }\
		   oldPtWin = ptWin;\
		 }\
	     }\
	   /* check timer */\
	   if(newEvent == 0)\
	       {\
		  EZ_SitForALittleBit(1000); \
     	          EZ_CheckDnDTimer();\
	       }\
	   if(!done && newEvent) /* not done yet */\
	     {\
	       newEvent = 0;\
	       if(moved)\
		 {\
		   if(grabed == 0)\
		     {\
		       Window child;\
		       XTranslateCoordinates(EZ_Display, widgetwin,\
					     RootWindow(EZ_Display, EZ_ScreenNum),\
					     0, 0,\
					     &wox,&woy, &child);\
		       iww = EZ_WidgetWidth(widget_);\
		       ihh = EZ_WidgetHeight(widget_);\
		       iarea  = iww * ihh;\
		       oldx = wox + x;\
		       oldy = woy + y;\
				  \
		       tx = - x;\
		       ty = - y;\
				  \
		       Sx = Ox = wox;\
		       Sy = Oy = woy;\
		       EZ_InitDrag(serial, EZ_DND_OBJ_IS_WIDGET, widgetwin,\
				   widget_, widget_, Sx,Sy, iww, ihh, oldx, oldy, tx, ty);\
		       if(EZ_DnDInfo.id == serial)				  \
			 {\
		            EZ_CallDragInitProcedure();\
			    if( EZ_DnDInfo.cursor == None)\
		               EZ_DnDInfo.cursor = EZ_WidgetDnDDragCursor(widget_);\
			    if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)\
			      snapshot = EZ_DND_DRAG_ICON_CURSOR;\
			    else if(iarea < 40000)\
			    snapshot = EZ_MakeSnapShot(widget_,0,0,0,iww,ihh);\
			    else snapshot = EZ_DND_DRAG_ICON_OUTLINE;\
			   {\
			     EZ_DnDInfo.actionType = actionType;\
			     EZ_DnDInfo.px = oldx;\
			     EZ_DnDInfo.py = oldy;\
			     EZ_DnDInfo.dragIconType = snapshot;\
			       {\
				 char *smsg; int smsg_len;\
				 EZ_GenerateDragStartedMessage(&smsg, &smsg_len);\
				 EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);\
			       }\
			  /*  EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0); */\
			   }\
			   if(EZ_PointerGrabed == 0)  /* grab the pointer ! */\
			     {\
			       if(XGrabPointer(EZ_Display,widgetwin, True,\
					       ButtonReleaseMask|ButtonPressMask|PointerMotionMask,\
					       GrabModeAsync, GrabModeAsync, None,\
                                               EZ_DnDInfo.cursor, CurrentTime)\
				  == GrabSuccess) EZ_PointerGrabed = 1;\
			     }	\
			   grabed = 1;\
			 }	\
		     }\
		   if(grabed)\
		     {\
		       int rootx,rooty,dx,dy;\
		       unsigned int mask;\
		       Window root, child;\
		       XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),\
				     &root,       /* root return win */\
				     &child,      /* child ret win   */\
				     &rootx, &rooty,   /* x, y in root    */\
				     &tx,&ty,            /* x, y in win     */\
				     &mask);  \
		       dx = rootx - oldx;  dy = rooty - oldy;\
		       oldx = rootx;   oldy = rooty;\
			      \
		       if(dx | dy)\
			 {\
			   Ox += dx; Oy += dy;\
			   if(snapshot != EZ_DND_DRAG_ICON_CURSOR)\
			     EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 1);\
			   else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);\
			   EZ_DnDInfo.px = oldx;\
			   EZ_DnDInfo.py = oldy;\
			   EZ_DnDInfo.ptWin = (unsigned long)ptWin;\
			   EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);\
			   if(snapshot != EZ_DND_DRAG_ICON_CURSOR)\
			     EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 0);\
			 }\
		     } /* grabed */\
		 }/* moved */\
	     } /* not done */			    \
	   else if(done) /* we are done */\
	     {\
	       if(EZ_PointerGrabed)\
		 {\
		   XUngrabPointer(EZ_Display,CurrentTime);\
		   EZ_PointerGrabed = 0;\
		 }\
	       if(grabed) /* dragged */\
		 {\
		   char *message;\
		   int length;\
		   Window commWin;\
		   \
		   if(snapshot != EZ_DND_DRAG_ICON_CURSOR)\
		     EZ_MoveSnapShotOrOutline(snapshot, 0,0,0,0,1);\
		   else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);\
		   if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);\
                   else commWin = 0L;\
		   EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */\
			      \
	           if(commWin != 0L)\
		     {\
		       if(helpRequested)\
			 {\
			   EZ_DnDSendCancelHelpMsg();\
			   helpRequested = 0;\
			 }\
			    \
		       if(dragCancelled == 0)\
			 {\
			   EZ_DnDInfo.actionType = actionType;\
			   EZ_GenerateDragIsDropedMessage(&message, &length);\
			   EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);\
			 }\
		       else  /* cancelled */\
			 {\
			   EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);\
			   EZ_AbortDrag();\
			 }\
		     }	\
 	           else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&\
			   dragCancelled == 0)\
		     {\
		       EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();\
		       if(encoder)\
			 {\
			   char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;\
			   \
			   EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;\
			   ok = (encoder->encoder) (encoder->object,\
						    encoder->data,\
						    &msg, &mlen, &needFree);\
			   if(ok != EZ_DND_FAILURE)\
			     {\
			       if((encoder->callback).callback)\
				 {\
				   EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;\
				   ((encoder->callback).callback)(encoder->object, \
								  (encoder->callback).data);\
				 }\
			       EZ_DnDInfo.id = 0; /* forget about this drag */   \
			     }\
			   else EZ_AbortDrag();\
			 }\
		       else  EZ_AbortDrag();\
		    }\
		   else    EZ_AbortDrag();\
		 }\
	     }\
	 }/* while */\
     }  /* is a drag src */\
}
/*****************************************************************************************/
