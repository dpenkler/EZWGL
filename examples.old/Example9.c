/************************** Example 9 ***********************/
#include "EZ.h"

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
  static EZ_Widget *dialogue = NULL;

  if(dialogue == NULL)
    {
      EZ_Widget *yes, *no;

      dialogue = EZ_CreateFrame(NULL, "Want to Quit?");
      EZ_ConfigureWidget(dialogue, EZ_TRANSIENT, 1, 0);
      yes = EZ_CreateButton(dialogue,"Yes", 0);
      no  = EZ_CreateButton(dialogue,"No", 0);   
      EZ_AddWidgetCallBack(yes, EZ_CALLBACK, yesCallBack, NULL, 0);
      EZ_AddWidgetCallBack(no, EZ_CALLBACK, noCallBack, dialogue, 0);
      EZ_SetWidgetPtrData(dialogue, no);  /* rember 'no' button */
    }
  EZ_DisplayWidget(dialogue);
  EZ_SetGrab(dialogue); 
  EZ_SetFocusTo((EZ_Widget *)EZ_GetWidgetPtrData(dialogue));
}

int main(int argc, char **argv)
{
  EZ_Widget *frame, *button, *buttonA;

  EZ_Initialize(argc,argv,0); 
  frame = EZ_CreateFrame(NULL, NULL);
  button  = EZ_CreateButton(frame, "Press me", 0);
  EZ_AddWidgetCallBack(button, EZ_CALLBACK, buttonCallBack, NULL, 0);
  buttonA = EZ_CreateButton(frame, "A Button", 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** Example 9 ***********************/

