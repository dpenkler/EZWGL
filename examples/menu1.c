
#include "EZ.h"

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
/***************************************************************************
 *
 *  Another way to create menus
 */

static EZ_MenuEntry menuEntries[] = {
  /* type                  name    label               accelerator  underline  retV   callback  cdata  v1 v2 v3 */
  {EZ_WIDGET_MENU_NORMAL_BUTTON,  NULL,  "normal Button",  NULL,       0,     100,   btnCb,    NULL,  0, 0, 0 },
  {EZ_WIDGET_MENU_NORMAL_BUTTON,  NULL,  "Another Button", "M-b",      0,     200,   btnCb,    NULL,  0, 0, 0 },
  {EZ_WIDGET_MENU_SEPARATOR,      NULL,  NULL,             NULL,       0,     0,     NULL,     NULL,  0, 0, 0 },
  {EZ_WIDGET_MENU_SUBMENU,        NULL,  "Walk Through",   "C-s",      0,     300,   NULL,    NULL,   0, 0, 0 },
     {EZ_WIDGET_MENU_NORMAL_BUTTON,  NULL, "S 1",            NULL,       0,     400,   btnCb,    NULL,  0, 0, 0 },
     {EZ_WIDGET_MENU_NORMAL_BUTTON,  NULL, "S 2",            NULL,       0,     500,   btnCb,    NULL,  0, 0, 0 },
     {EZ_WIDGET_MENU_SEPARATOR,      NULL,  NULL,            NULL,       0,     0,     NULL,     NULL,  0, 0, 0 },
     {EZ_WIDGET_MENU_CHECK_BUTTON,   NULL, "C 1",            NULL,       0,     600,   cbtnCb,   NULL,  0, 1, 1 },
     {EZ_WIDGET_MENU_CHECK_BUTTON,   NULL, "C 2",            NULL,       0,     700,   cbtnCb,   NULL,  0, 1, 0 },
     {EZ_WIDGET_MENU_SEPARATOR,      NULL,  NULL,            NULL,       0,     0,     NULL,     NULL,  0, 0, 0 },
     {EZ_WIDGET_MENU_RADIO_BUTTON,   NULL, "R 1",            "C-r",      0,     800,   rbtnCb,   NULL,  5, 0, 0 },
     {EZ_WIDGET_MENU_RADIO_BUTTON,   NULL, "R 2",            NULL,       0,     900,   rbtnCb,   NULL,  5, 1, 0 },
     {EZ_WIDGET_MENU_RADIO_BUTTON,   NULL, "R 3",            NULL,       0,     1000,  rbtnCb,   NULL,  5, 2, 2 },
     {0,                             NULL, NULL,             NULL,       0,     0,     NULL,     NULL,  0, 0, 0 },
  {EZ_WIDGET_MENU_NORMAL_BUTTON,  NULL,  "Last Button", NULL,          0,     2000,  btnCb,    NULL,  0, 0, 0 },
  {0,                      NULL, NULL,             NULL,       0,     0,     NULL,     NULL,  0, 0, 0 }
};

int main(int ac, char **av)
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

  menu2 = EZ_CreateMenuFromList(menuEntries);
  EZ_SetMenuButtonMenu(mbtn2, menu2);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}


