/************************** Example 10 ***********************/
#include "EZ.h"

EZ_Widget *Dialogue;         /* a dialogue       */
Atom       WMProtocolsAtom;  /* WM_PROTOCOLS     */
Atom       DeleteWindowAtom; /* WM_DELETE_WINDOW */

void yesCallBack(EZ_Widget *widget, void *data)
{
  EZ_Shutdown();
  exit(0);
}

void noCallBack(EZ_Widget *widget, void *data)
{
  if(widget)
    {
      EZ_Widget *toplevel = (EZ_Widget *)data;

      EZ_HideWidget(toplevel); 
      EZ_ReleaseGrab();
    }
}

void buttonCallBack(EZ_Widget *widget, void *data)
{
  EZ_DisplayWidget(Dialogue);
  EZ_SetGrab(Dialogue); 
  EZ_SetFocusTo((EZ_Widget *)EZ_GetWidgetPtrData(Dialogue));
}

void clientMessageHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  XClientMessageEvent *ev = (XClientMessageEvent *)xev;      
      
  if(ev->message_type == WMProtocolsAtom)
    {
      Atom c = (ev->data.l)[0];
      if(c == DeleteWindowAtom)
	{
	  EZ_DisplayWidget(Dialogue);
	  EZ_SetGrab(Dialogue); 
	  EZ_SetFocusTo((EZ_Widget *)EZ_GetWidgetPtrData(Dialogue));
	}
    }
}

int main(int argc, char **argv)
{
  EZ_Widget *frame, *button, *buttonA;
  EZ_Widget *yes, *no;

  EZ_Initialize(argc,argv,0); 
  DeleteWindowAtom = EZ_GetAtom("WM_DELETE_WINDOW");
  WMProtocolsAtom = EZ_GetAtom("WM_PROTOCOLS");

  frame = EZ_CreateFrame(NULL, NULL);
  button  = EZ_CreateButton(frame, "Press me", 0);
  EZ_AddWidgetCallBack(button, EZ_CALLBACK, buttonCallBack, NULL, 0);
  buttonA = EZ_CreateButton(frame, "A Button", 0);

  Dialogue = EZ_CreateFrame(NULL, "Want to Quit?");
  EZ_ConfigureWidget(Dialogue, EZ_TRANSIENT, 1, 0);
  yes = EZ_CreateButton(Dialogue,"Yes", 0);
  no  = EZ_CreateButton(Dialogue,"No", 0);   
  EZ_AddWidgetCallBack(yes, EZ_CALLBACK, yesCallBack,NULL, 0);
  EZ_AddWidgetCallBack(no,  EZ_CALLBACK, noCallBack, Dialogue,0);
  EZ_SetWidgetPtrData(Dialogue, no); 

  EZ_SetClientMessageHandler(clientMessageHandler, NULL);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** Example 10 ***********************/

