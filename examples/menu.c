
#include "EZ.h"

EZ_Widget *tmenu, *ssmenu;

static quit(EZ_Widget *widget, void *data)
{
  EZ_Shutdown();
  exit(0);
}

void menuCb(EZ_Widget *widget, void *data)
{
  int ret = EZ_GetWidgetReturnedData(widget);
  printf("MenuCallback: returnedValue=%d\n",ret);
  if(ret == 2) EZ_DoPopup(ssmenu, -1);
}

void smenuCb(EZ_Widget *widget, void *data)
{
  int ret = EZ_GetWidgetReturnedData(widget);
  printf("SubMenuCallback: returnedValue=%d\n",ret);
  if(ret == 2) EZ_DoPopup(ssmenu, -1);
}


void rbtnCb(EZ_Widget *widget, void *data)
{
  int gid =  EZ_GetRadioButtonGroupID(widget);
  int gvalue = EZ_GetRadioButtonGroupVariableValue(widget);
  int ret = EZ_GetWidgetReturnedData(widget);
  printf("RadioButton, gid= %d, VarValue=%d, returnValue=%d\n", gid, gvalue,ret);
}
void cbtnCb(EZ_Widget *widget, void *data)
{
  int value;
  int state = EZ_GetCheckButtonState(widget, &value);
  int ret = EZ_GetWidgetReturnedData(widget);
  printf("CheckButton, state=%s, value=%d, returnValue=%d\n", state?"ON":"OFF", value,ret);  
}
void btnCb(EZ_Widget *widget, void *data)
{
  int ret = EZ_GetWidgetReturnedData(widget);
  printf("NormalButton, returned Value =%d\n", ret);
}



main(int ac, char **av)
{
  EZ_Widget *frame, *mbtn1, *mbtn2, *mbtn3, *frame4, *btn;
  EZ_Widget *menu1, *menu2, *menu3, *smenu;

  EZ_Initialize(ac, av, 0);

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Buttons",          "buttons",
                              EZ_IPADX,           4,
                              EZ_SIDE,            EZ_TOP,
                              0);
  mbtn2   = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BUTTON,    frame,
                              "Btn1",             "btn2",
                               EZ_LABEL_STRING,    "Another Menu Button",
                               EZ_FOREGROUND,      "blue",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_UNDERLINE,       0,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_INDICATOR_TYPE,  EZ_MENU_BUTTON_INDICATOR,
                               EZ_INDICATOR_COLOR, "green",
                               EZ_FOREGROUND,      "blue",
                               EZ_BG_IMAGE_FILE, "bg.gif",
                              0);
  smenu= EZ_CreateSimpleMenu("%Twalk through %F|test |%l| radio1 %r[1,0,0] %f|radio 2 %r[1,1,0] %f|%l| check %k[1,0,1] %f|check2 %k[1,0,0] %f|", smenuCb, NULL, rbtnCb, NULL,rbtnCb, NULL,cbtnCb, NULL,cbtnCb, NULL);
  tmenu = menu2 = EZ_CreateSimpleMenu("%Tmenu2 %F|||%m a submenu||separator|%l||something %f|%l|check btn %k[1,0,1]%f|check btn 2 %k[1,0,0] %f|%l|Radio button %r[2,0,0] %f|RadioBtn 2 %r[2,1,0] %f|RadioBtn 3%r[2,2,0]%f|",menuCb, NULL, smenu, btnCb, NULL,  cbtnCb, NULL, cbtnCb, NULL, rbtnCb, NULL, rbtnCb, NULL, rbtnCb, NULL);
  EZ_SetMenuButtonMenu(mbtn2, menu2);

  ssmenu= EZ_CreateSimpleMenu("%T ssssss %F|test |%l| |%l| check %k[1,0,1] %f|check2 %k[1,0,0] %f|", smenuCb, NULL, rbtnCb, NULL,rbtnCb, NULL,cbtnCb, NULL,cbtnCb, NULL);

  frame4 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame4",           "frame4",
                              EZ_LABEL_STRING,    " ",     
                              EZ_BACKGROUND,      "#00cccc",
                              EZ_DOCKABLE,        True,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame4,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}


