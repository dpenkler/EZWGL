/************************** resource editor ****************************/
#include "EZ.h"
static char *resources[] = {
  "*foreground",
  "*background",
  "*bgImageFile",
  "*bgImageFileB",
  "*fontName",
  "*highlight",
  "*labelOffset",
  "*sliderDepression",
  "*focusHighlightColor",
  "*nothing",
};

/**********************************************************************************/
EZ_Widget *toplevel;  
EZ_Widget *resNameEntry;
EZ_Widget *resValueEntry;
/**********************************************************************************/
static void sendCb(EZ_Widget *widget, void *data);
static void doexit(EZ_Widget *w, void *d) { EZ_Shutdown(); exit(0);}
/**********************************************************************************/

main(int ac, char **av)
{
  EZ_Widget *tmp, *ump, *tframe, *rframe;
  EZ_Initialize(ac, av, 0);

  toplevel  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,   NULL,
                                 "toplevel",        NULL,
                                 EZ_LABEL_STRING,   "Resource Editor",
                                 EZ_FONT_NAME,      "12x24",
                                 EZ_PADX,            0,
                                 EZ_PADY,            0,
                                 EZ_SIZE,            600, 200,
                                 EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 10,  0, 0,
                                 EZ_GRID_CONSTRAINS, EZ_COLUMN, 1, 10,  100, 0,
                                 EZ_GRID_CONSTRAINS, EZ_COLUMN, 2, 10,  100, 0,
                                 EZ_GRID_CONSTRAINS, EZ_ROW,    2, 10,  0, 0,
                                 0);

  tmp         = EZ_CreateWidget(EZ_WIDGET_LABEL, toplevel,
                                EZ_LABEL_STRING, "Resource Name:",
                                EZ_GRID_CELL_GEOMETRY,  0, 0, 1, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                0);
                                
  resNameEntry = EZ_CreateWidgetXrm(EZ_WIDGET_OPTIONAL_ENTRY, toplevel,
                                    "resourceNameEntry",     NULL,
                                    EZ_OPTIONAL_ENTRY_STYLE, 1,
                                    EZ_GRID_CELL_GEOMETRY,  1, 0, 2, 1,
                                    EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                    0);

  EZ_SetOptionalEntryOptionalItems(resNameEntry, resources, 10);

  tmp         = EZ_CreateWidget(EZ_WIDGET_LABEL, toplevel,
                                EZ_LABEL_STRING, "Resource Value:",
                                EZ_GRID_CELL_GEOMETRY,  0, 1, 1, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                0);
  resValueEntry = EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY, toplevel,
                                "resourceValueEntry", NULL,
                                EZ_GRID_CELL_GEOMETRY,  1, 1, 2, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                EZ_CALLBACK,sendCb, NULL,
                                0);
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, toplevel,
                            "send", NULL,
                            EZ_LABEL_STRING,  "Send",
                            EZ_UNDERLINE,      0,
                            EZ_CALLBACK,       sendCb, NULL,
                            EZ_GRID_CELL_GEOMETRY,  1, 3, 1, 1,
                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                            0);  
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, toplevel,
                            "exit", NULL,
                            EZ_LABEL_STRING,  "Exit",
                            EZ_UNDERLINE,      0,
                            EZ_CALLBACK,       doexit, NULL,
                            EZ_GRID_CELL_GEOMETRY,  2, 3, 1, 1,
                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                            0);  

  EZ_DisplayWidget(toplevel);
  EZ_EventMainLoop();
  }
/***********************************************************************************************/
static void sendCb(EZ_Widget *widget, void *data)
{
  Atom   mtype = EZ_RESOURCE_EDITOR_ATOM;
  char   str[512];
  char   *name = EZ_GetEntryString(resNameEntry);
  char   *value= EZ_GetEntryString(resValueEntry);

  sprintf(str, "%s: %s", name, value);
  EZ_BroadcastMessage(mtype,        /* msg type */
                      str,         /* the message */
                      strlen(str),
                      EZ_RE_SET_RESOURCE2,
                      12343
                      );
}

/*********************************************************************/
