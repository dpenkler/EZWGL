/************************** resource editor ****************************/
#include "EZ.h"
static char *tick_xpm[] = {
/* width height num_colors chars_per_pixel */
"    16    16        2            1",
/* colors */
". c none",
"# c #ff0000",
/* pixels */
"................",
"...............#",
".............##.",
"............##..",
"...........##...",
"..........##....",
"....##...###....",
"...####.###.....",
"..#########.....",
"...#######......",
"....######......",
".....####.......",
"......###.......",
".......#........",
"................",
"................"
};


/* this is the clientData recorded in all items in the widget tree */
typedef struct {
  unsigned long     id;
  unsigned long     win;
  XrmQuark          iname;
  XrmQuark          cname;
  char              *info;
} nodeData;

/**********************************************************************************/

EZ_Widget *toplevel;  
EZ_Widget *menubar;
EZ_Widget *treeWidget;
EZ_Widget *resBox;
EZ_Widget *resDialogue;
EZ_Widget *resFrame;
EZ_Widget *resNameEntry;
EZ_Widget *resEntry;
EZ_Widget *resLabels[10][4];

EZ_Widget *treeMenu;
EZ_Widget *getTree;
EZ_Widget *getOneTree;
EZ_Widget *showName, *showCName;

EZ_Widget *helpMenu;
EZ_LabelPixmap *tickXpm;

Window currentClient = None;
int    senderIsSelf = 0;

char   resFileName[256];
XrmQuark currentResIName, currentResCName;
/**********************************************************************************/
static int gmData[] = {EZ_FILL_MODE, EZ_FILL_BOTH, EZ_LABEL_POSITION, EZ_LEFT, 0};
/**********************************************************************************/
static void getTreeCb(EZ_Widget *widget, void *data);
static void showTreeCb(EZ_Widget *widget, void *data);
static void treeCb(EZ_Widget *widget, void *data);
static void workareaCb(EZ_Widget *widget, void *data);
static void resLabelsCb(EZ_Widget *widget, void *data);
static void setResourceCb(EZ_Widget *widget, void *data);
static void setSaveFileCb(EZ_Widget *widget, void *data);
static void saveFileCb(EZ_Widget *widget, void *data);
static void nameEntryCb(EZ_Widget *widget, void *data);
static void valueEntryCb(EZ_Widget *widget, void *data);
static void blinkCb(EZ_Widget *widget, void *data);
static void Warning(char *lab1, char *lab2);
static void makeResourceDialogue();
static void highlightTreeNodes();
static void unhighlightTreeNodes();
static void showInfo(EZ_Widget *w, void *d);
static void doexit(EZ_Widget *w, void *d) { EZ_Shutdown(); exit(0);}
/**********************************************************************************/

main(int ac, char **av)
{
  EZ_Widget *tmp, *ump, *tframe, *rframe;
  EZ_Initialize(ac, av, 0);
  tickXpm = EZ_CreateLabelPixmapFromXpmData(tick_xpm);

  toplevel  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                                 "toplevel",        NULL,
                                 EZ_PADX,            0,
                                 EZ_PADY,            0,
                                 EZ_SIZE,            800, 500,
                                 EZ_FILL_MODE,       EZ_FILL_BOTH,
                                 EZ_ORIENTATION,     EZ_VERTICAL,
                                 0);
  /* children are: menubar and tframe */

  menubar = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BAR, toplevel, 
                               "menubar", NULL,
                               0);

  tframe = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, toplevel,
                              "tframe",        NULL,
                              EZ_PADX,         0,
                              EZ_PADY,         0,
                              EZ_FILL_MODE,    EZ_FILL_BOTH,  
                              0);
  
  /* The first menu item on menubar */
  treeMenu = EZ_CreateWidgetXrm(EZ_WIDGET_MENU, NULL,
                                "treeMenu", NULL,
                                0);
  getTree  = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_NORMAL_BUTTON, treeMenu,
                                "getTree",       NULL,
                                EZ_LABEL_STRING, "Get Tree",
                                EZ_UNDERLINE,     0,
                                EZ_CALLBACK,      getTreeCb, NULL,
                                0);
  getOneTree  = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_NORMAL_BUTTON, treeMenu,
                                   "getOneTree",      NULL,
                                   EZ_LABEL_STRING, "Get One Tree",
                                   EZ_UNDERLINE,     4,
                                   EZ_CALLBACK,      getTreeCb, (void *)0x1,
                                   0);
  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_SEPARATOR, treeMenu, 0);

  showName = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_RADIO_BUTTON, treeMenu,
                                "showName",       NULL,
                                EZ_LABEL_STRING,  "Show Name",
                                EZ_UNDERLINE,     5,
                                EZ_RADIO_BUTTON_GROUP, 123,
                                EZ_RADIO_BUTTON_VALUE, 4,
                                EZ_CALLBACK,     showTreeCb, NULL,
                                0);
  EZ_DisableWidget(showName);
  showCName = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_RADIO_BUTTON, treeMenu,
                                 "showClass",       NULL,
                                 EZ_LABEL_STRING,  "Show Class",
                                 EZ_UNDERLINE,     5,
                                 EZ_RADIO_BUTTON_GROUP, 123,
                                 EZ_RADIO_BUTTON_VALUE, 3,
                                 EZ_CALLBACK,     showTreeCb, NULL,
                                 0);

  EZ_DisableWidget(showCName);
  tmp = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_RADIO_BUTTON, treeMenu,
                           "showWidget",     NULL,
                           EZ_LABEL_STRING,  "Show Id",
                           EZ_UNDERLINE,     5,
                           EZ_RADIO_BUTTON_GROUP, 123,
                           EZ_RADIO_BUTTON_VALUE, 1,
                           EZ_CALLBACK,      showTreeCb, NULL,
                           0);
  EZ_DisableWidget(tmp);
  tmp = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_RADIO_BUTTON, treeMenu,
                           "showWindow",       NULL,
                           EZ_LABEL_STRING,  "Show Window",
                           EZ_UNDERLINE,     5,
                           EZ_RADIO_BUTTON_GROUP, 123,
                           EZ_RADIO_BUTTON_VALUE, 2,
                           EZ_CALLBACK,      showTreeCb, NULL,
                           0);
  EZ_DisableWidget(tmp);
  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_SEPARATOR, treeMenu, 0);
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_NORMAL_BUTTON, treeMenu,
                            "blink",       NULL,
                            EZ_LABEL_STRING, "Blink Selection",
                            EZ_UNDERLINE,     0,
                            EZ_CALLBACK,      blinkCb, NULL,
                            0);

  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_NORMAL_BUTTON, treeMenu,
                            "exit",       NULL,
                            EZ_LABEL_STRING, "Exit",
                            EZ_UNDERLINE,     0,
                            EZ_CALLBACK,      doexit, NULL,
                            0);

  EZ_MenuBarInsertItemAndMenu(menubar,"Tree", 0, treeMenu, NULL, 0);

  EZ_MenuBarAddItemAndMenu(menubar, NULL, -1, NULL);  /* separator */

  helpMenu = EZ_CreateWidgetXrm(EZ_WIDGET_MENU, NULL,
                                "helpMenu", NULL,
                                0);
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_NORMAL_BUTTON, helpMenu,
                            "help",       NULL,
                            EZ_LABEL_STRING, "Info",
                            EZ_UNDERLINE,     0,
                            EZ_CALLBACK,      showInfo, NULL,
                            0);
  EZ_MenuBarInsertItemAndMenu(menubar,"Help", 0, helpMenu, NULL, 0);


  treeWidget = EZ_CreateWidgetXrm(EZ_WIDGET_TREE, tframe,
                                  "widgetTree",     NULL,
                                  EZ_CALLBACK,      treeCb, NULL,
                                  EZ_WIDTH,         200,
                                  0);
  tmp = EZ_CreateWidget(EZ_WIDGET_PANE_HANDLE, tframe, 0);

  rframe = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, tframe,
                              "rframe",        NULL,
                              EZ_PADX,         0,
                              EZ_PADY,         0,
                              EZ_FILL_MODE,    EZ_FILL_BOTH,  
                              EZ_ORIENTATION,  EZ_VERTICAL,
                              0);

  resBox =  EZ_CreateWidgetXrm(EZ_WIDGET_WORK_AREA, rframe,
                               "ResourceBox",     "resourceBox",
                               EZ_ITEM_MOVABLE,    False,
                               EZ_IPADX,           2,
                               EZ_IPADY,           2,                               
                               EZ_OPTIONAL_VSCROLLBAR, False,
                               EZ_CALLBACK,        workareaCb, NULL,
                               0);
  EZ_SetWorkAreaGeometryManager(resBox,
                                EZ_WorkAreaGeometryManagerDefault,
                                gmData);
  tmp = EZ_CreateWidget(EZ_WIDGET_PANE_HANDLE, rframe, 0);

  resDialogue = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, rframe,
                                   "ResourceDialogue",     "resourceDialogue",
                                   EZ_LABEL_STRING, "Resource Editor ",
                                   EZ_HEIGHT,  0,
                                   EZ_GRID_CONSTRAINS, EZ_COLUMN, 0,  10,  100, 0,
                                   EZ_GRID_CONSTRAINS, EZ_COLUMN, 11, 10,  100, 0,
                                   0);    
  makeResourceDialogue();
  EZ_DisableWidgetTree(resDialogue);
  
  EZ_DisplayWidget(toplevel);
  EZ_EventMainLoop();
}
/*********************************************************************/
static void hideWidget(EZ_Widget *widget, void *data)
{
  EZ_HideWidget((EZ_Widget *)data);
}
/*********************************************************************/
void makeResourceDialogue()
{
  int i;
  EZ_Widget *frame, *tmp;
  char   iname[128];

  resFrame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, resDialogue,
                                "resourceFrame", NULL,
                                EZ_FOREGROUND,   "blue",
                                EZ_FILL_MODE, EZ_FILL_BOTH,
                                EZ_TEXT_LINE_LENGTH, 128,
                                EZ_ORIENTATION, EZ_VERTICAL,
                                EZ_GRID_CELL_GEOMETRY,  0, 5, 12, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                EZ_GRID_CONSTRAINS, EZ_COLUMN, 1, 200,  100, 0,
                                0);
  
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME, resDialogue,
                           EZ_FILL_MODE, EZ_FILL_BOTH,
                           EZ_GRID_CELL_GEOMETRY,  0, 6, 12, 1,
                           EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                           0);
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, frame,
                            "apply", NULL,
                            EZ_LABEL_STRING,  "Apply",
                            EZ_UNDERLINE,      0,
                            EZ_CALLBACK,       setResourceCb, NULL,
                            0);  
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, frame,
                            "save", NULL,
                            EZ_LABEL_STRING,  "Set and Install",
                            EZ_UNDERLINE,      8,
                            EZ_CALLBACK,        setResourceCb, (void *)0x1,
                            0);  
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, frame,
                            "save", NULL,
                            EZ_LABEL_STRING,  "Save",
                            EZ_UNDERLINE,      0,
                            EZ_CALLBACK,       saveFileCb, NULL,
                            0);  
  tmp  = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, frame,
                            "setFile", NULL,
                            EZ_LABEL_STRING,  "Set Save File",
                            EZ_UNDERLINE,      9,
                            EZ_CALLBACK,       setSaveFileCb, NULL,
                            0);  


  tmp         = EZ_CreateWidget(EZ_WIDGET_LABEL, resFrame,
                                EZ_LABEL_STRING, " Name:",
                                EZ_GRID_CELL_GEOMETRY,  0, 0, 1, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                0);
  resNameEntry = EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY, resFrame,
                                    "resourceNameEntry", NULL,
                                    EZ_GRID_CELL_GEOMETRY,  1, 0, 1, 1,
                                    EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                    EZ_CALLBACK, nameEntryCb, NULL,
                                    0);
  tmp         = EZ_CreateWidget(EZ_WIDGET_LABEL, resFrame,
                                EZ_LABEL_STRING, "Value:",
                                EZ_GRID_CELL_GEOMETRY,  0, 1, 1, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                0);

  resEntry = EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY, resFrame,
                                "resourceValueEntry", NULL,
                                EZ_GRID_CELL_GEOMETRY,  1, 1, 1, 1,
                                EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                EZ_CALLBACK, valueEntryCb, NULL,
                                0);
  for(i = 0; i < 10; i++)
    {
      sprintf(iname, "rlabel%d", i);
      resLabels[i][0] =  EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON, resDialogue,
                                            iname,           NULL,
                                            EZ_LABEL_STRING, "Unknown",
                                            EZ_LABEL_POSITION, EZ_LEFT,
                                            EZ_RADIO_BUTTON_GROUP, 124+2*i+1,
                                            EZ_RADIO_BUTTON_VALUE, 0,
                                            EZ_GRID_CELL_GEOMETRY,  (1+i), 1, 1, 1,
                                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                            EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                                            EZ_BORDER_WIDTH, 1,
                                            EZ_PADY, 1,
                                            EZ_CALLBACK,      resLabelsCb, NULL,
                                            0);
      EZ_DeActivateWidget(resLabels[i][0]);
      resLabels[i][1] =  EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON, resDialogue,
                                            iname,           NULL,
                                            EZ_LABEL_STRING, "Unknown",
                                            EZ_LABEL_POSITION, EZ_LEFT,
                                            EZ_RADIO_BUTTON_GROUP, 124+2*i+1,
                                            EZ_RADIO_BUTTON_VALUE, 1,
                                            EZ_GRID_CELL_GEOMETRY,  (1+i), 2, 1, 1,
                                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                            EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                                            EZ_BORDER_WIDTH, 1,
                                            EZ_PADY, 1,
                                            EZ_CALLBACK,      resLabelsCb, NULL,
                                            0);
      EZ_DeActivateWidget(resLabels[i][1]);
      resLabels[i][2] =  EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON, resDialogue,
                                            iname,           NULL,
                                            EZ_LABEL_STRING, "Any Widget",
                                            EZ_LABEL_POSITION, EZ_LEFT,
                                            EZ_RADIO_BUTTON_GROUP, 124+2*i+1,
                                            EZ_RADIO_BUTTON_VALUE, 2,
                                            EZ_GRID_CELL_GEOMETRY,  (1+i), 3, 1, 1,
                                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                            EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                                            EZ_BORDER_WIDTH, 1,
                                            EZ_PADY, 1,
                                            EZ_CALLBACK,      resLabelsCb, NULL,
                                            0);
      EZ_DeActivateWidget(resLabels[i][2]);
      resLabels[i][3] =  EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON, resDialogue,
                                            iname,           NULL,
                                            EZ_LABEL_STRING, "Any Chain",
                                            EZ_LABEL_POSITION, EZ_LEFT,
                                            EZ_RADIO_BUTTON_GROUP, 124+2*i+1,
                                            EZ_RADIO_BUTTON_VALUE, 3,
                                            EZ_GRID_CELL_GEOMETRY,  (1+i), 4, 1, 1,
                                            EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                            EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                                            EZ_BORDER_WIDTH, 1,
                                            EZ_PADY, 1,
                                            EZ_CALLBACK,      resLabelsCb, NULL,
                                            0);
      EZ_DeActivateWidget(resLabels[i][3]);
    }
}
/***********************************************************************************************/
static void timeoutCb(void *label, void *data)
{
  int what = (int)data;
  char *ptr;
  switch(what)
    {
    case EZ_RE_GET_TREE1:    ptr ="EZ_RE_GET_TREE1";    break;
    case EZ_RE_GET_TREE:     ptr ="EZ_RE_GET_TREE";     break;
    case EZ_RE_GET_RESOURCE: ptr ="EZ_RE_GET_RESOURCE"; break;
    case EZ_RE_SET_RESOURCE: ptr ="EZ_RE_SET_RESOURCE"; break;
    case EZ_RE_BLINK_WIDGET: ptr ="EZ_RE_BLINK_WIDGET"; break;
    default: ptr = "Unknown message type"; break;
    }
  if(currentClient != None && EZ_WindowExist(currentClient)== 0)
    {
      currentClient = None;
      EZ_SetListTreeWidgetTree(treeWidget, NULL);
      EZ_WorkAreaClear(resBox);      
      EZ_DisableWidgetTree(resDialogue);
      EZ_SetWidgetIntData(resDialogue, 0);
    }
  Warning("No answer for message", ptr);
}

static void msgHandler(EZ_Message *msg, void *data)
{
  if(msg->messageLength > 0)
    {
      switch(msg->messageId)
        {
        case EZ_RE_GET_TREE+100:  /* reply to getTree    */
        case EZ_RE_GET_TREE1+100: /* reply to getOneTree */
          {
            EZ_TreeNode *tree = EZ_ParseWidgetTree(msg->message);
            EZ_SetListTreeWidgetTree(treeWidget, tree);
            EZ_WorkAreaClear(resBox);
            EZ_SetRadioButtonGroupVariableValue(showName, 4);
            EZ_EnableWidgetTree(treeMenu);
            currentClient = msg->sender;
            senderIsSelf = msg->senderIsSelf;
            EZ_DisableWidgetTree(resDialogue);
            EZ_SetWidgetIntData(resDialogue, 0);
          }
        break;
        case EZ_RE_GET_RESOURCE+100: /* reply of get resources */
          {
            int nn;
            EZ_Item **items = EZ_ParseResourcesTable(msg->message, &nn);
            if(nn > 0)
              {
                EZ_WorkAreaClear(resBox);
                EZ_WorkAreaInsertItems(resBox,items, nn);
                EZ_DisableWidgetTree(resDialogue);
                EZ_SetWidgetIntData(resDialogue, 0);
                unhighlightTreeNodes();
              }
          }
        break;
        }
    }
  EZ_DeleteMessageHandler(msg->messageType, msg->messageId, data, 
                          (EZ_MessageHandler) msgHandler);
}
/***********************************************************************************************/
static void getTreeCb(EZ_Widget *widget, void *data)
{
  Window win = EZ_SelectWindowWithButtonClick();
  Atom   mtype = EZ_RESOURCE_EDITOR_ATOM;
  int    replyId = EZ_RE_GET_TREE+100;
  char   str[32];

  if(win != None)
    {
      EZ_RegisterMessageHandler(mtype,              /* type of msg it handles */
                                replyId,            /* this is the replyId   */
                                data,               /* client data to be passed to handler */
                                (EZ_MessageHandler) msgHandler,  /* the handler     */
                                1,           /* expire in one second   */
                                timeoutCb,   /* what to do when handler expires */
                                (void *) (data? EZ_RE_GET_TREE1: EZ_RE_GET_TREE)
                                );
      /* broadcast a message to find the client 
       * This is the first query message. The res editor
       * does not know anything about the client. The message
       * include only the window id of the last selected window.
       * The client will respond only if the window id belongs
       * to it.
       */
      sprintf(str, "%lx", (unsigned long)win);
      EZ_BroadcastMessage(mtype,        /* msg type */
                          str,         /* the message */
                          strlen(str),
                          data?EZ_RE_GET_TREE1: EZ_RE_GET_TREE,
                          replyId
                          );
    }
}
/*********************************************************************/
static int   showWhich = 0;
static void  traverseF(EZ_TreeNode *node)
{
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      nodeData *nd = (nodeData *)EZ_GetItemPtrData(item);
      char s[80];
      switch(showWhich)
        { 
        case 1: sprintf(s, "id:  0x%lx", nd->id);     break;  /* show id */
        case 2: sprintf(s, "win: 0x%lx", nd->win);    break;  /* etc */
        case 3: strcpy(s, EZ_QuarkToString(nd->cname));  break;
        default:strcpy(s, EZ_QuarkToString(nd->iname));  break;
        }
      EZ_ConfigureItem(item, EZ_LABEL_STRING, s, 0);
    }
}
/***********************************************************************************************/
static void showTreeCb(EZ_Widget *widget, void *data)
{
  EZ_TreeNode *node = EZ_GetListTreeWidgetTree(treeWidget);
  if(node)
    {
      EZ_FreezeWidget(treeWidget);
      showWhich = EZ_GetRadioButtonGroupVariableValue(showName);
      EZ_TreeTraverseTree(node, traverseF, NULL);
      EZ_UnFreezeWidget(treeWidget);
    }
}

/*********************************************************************/
static void treeCb(EZ_Widget *widget, void *data)
{
  EZ_TreeNode *node = EZ_GetListTreeWidgetSelection(widget);
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      nodeData *nd = (nodeData *)EZ_GetItemPtrData(item);
      Atom   mtype = EZ_RESOURCE_EDITOR_ATOM;
      int    replyId = EZ_RE_GET_RESOURCE+100;      
      char   message[256];
      EZ_RegisterMessageHandler(mtype,              /* type of msg it handles */
                                replyId,            /* this is the replyId   */
                                data,               /* client data to be passed to handler */
                                (EZ_MessageHandler) msgHandler,  /* the handler     */
                                1,           /* expire in one second   */
                                timeoutCb,   /* what to do when handler expires */
                                (void *)EZ_RE_GET_RESOURCE/* client data to be passed to timeoutCb */
                                );      
      sprintf(message, "%lx", nd->id);
      EZ_SendMessage(mtype,message, strlen(message),EZ_RE_GET_RESOURCE,
                     currentClient, replyId, senderIsSelf);
    }
}
/****************************************************************************/
static void blinkCb(EZ_Widget *widget, void *data)
{
  EZ_TreeNode *node = EZ_GetListTreeWidgetSelection(treeWidget);
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      nodeData *nd = (nodeData *)EZ_GetItemPtrData(item);
      Atom   mtype = EZ_RESOURCE_EDITOR_ATOM;
      int    replyId = EZ_RE_BLINK_WIDGET+100;      
      char   message[256];
      EZ_RegisterMessageHandler(mtype,              /* type of msg it handles */
                                replyId,            /* this is the replyId   */
                                data,               /* client data to be passed to handler */
                                (EZ_MessageHandler) msgHandler,  /* the handler     */
                                1,           /* expire in one second   */
                                timeoutCb,   /* what to do when handler expires */
                                (void *)EZ_RE_BLINK_WIDGET
                                );      
      sprintf(message, "%lx", nd->id);
      EZ_SendMessage(mtype,message, strlen(message),EZ_RE_BLINK_WIDGET,
                     currentClient, replyId, senderIsSelf);
    }
}
/****************************************************************************/

static void updateResourceName()
{
  int cnt = EZ_GetWidgetIntData(resDialogue);
  if(cnt > 0)
    {
      EZ_TreeNode *tnode = EZ_GetListTreeWidgetSelection(treeWidget);
      EZ_Item     *ritem = EZ_GetWorkAreaSelection(resBox);
      EZ_TreeNode *pnode=tnode;
      char *ptr, str[512];
      char *names[12], *cnames[12];
      int  rvalues[12];
      EZ_Item *path[32];
      int j, i = 31;
      nodeData *nd;
      str[0] = '\0';
      while(pnode)
        {
          path[i] = EZ_TreeNodeGetItem(pnode);
          i--;
          pnode = EZ_TreeNodeGetParent(pnode);
        }
      j = 0; while(i < 32) { path[j++] = path[++i];};
      for(i = 0; i < cnt; i++)
        {
          rvalues[i] = EZ_GetRadioButtonGroupVariableValue(resLabels[i][0]);
          nd = (nodeData *)EZ_GetItemPtrData(path[i]);
          names[i] = EZ_QuarkToString(nd->iname);
          cnames[i] = EZ_QuarkToString(nd->cname);
        }
      rvalues[cnt] = EZ_GetRadioButtonGroupVariableValue(resLabels[cnt][0]);
      EZ_GetLabelItemStringInfo(ritem, &ptr, &j);
      names[cnt] = ptr;
      for(i = 0; i < cnt; i++)
        {
          switch(rvalues[i])
            {
            case 0: 
              if(i == 0  || rvalues[i-1] != 3) 
                strcat(str, ".");
              strcat(str, names[i]);
              break;
            case 1: 
              if(i == 0  || rvalues[i-1] != 3) 
                strcat(str, ".");
              strcat(str, cnames[i]);
              break;
            case 2:
              if(i > 0 && rvalues[i-1] == 3) 
                strcat(str, "?"); 
              else
                strcat(str, ".?"); 
              break;
            case 3:
              if(i > 0 && rvalues[i-1] == 3) break;
              else strcat(str, "*"); 
              break;
            }
        }
      if(rvalues[cnt-1] != 3)
        {
          if(rvalues[cnt] == 0) strcat(str,".");  
          else strcat(str,"*");  
        }
      strcat(str,names[cnt]); 
      EZ_SetEntryString(resNameEntry, str);
      nd = (nodeData *)EZ_GetItemPtrData(ritem);
      currentResIName = nd->iname;
      currentResCName = nd->cname;
      sprintf(str,"Value Type: (%s)", nd->info);
      EZ_ConfigureWidget(resFrame, EZ_LABEL_STRING, str, 0);
      highlightTreeNodes();
    }
  else
    {
      EZ_SetEntryString(resNameEntry, " ");
      EZ_ConfigureWidget(resFrame, EZ_LABEL_STRING, "Value Type:", 0);
    }
}
/****************************************************************************/
static void  resLabelsCb(EZ_Widget *widget, void *data)
{
  updateResourceName();
}
/****************************************************************************/

static void workareaCb(EZ_Widget *widget, void *data)
{
  static EZ_Item *previousSelection = NULL;
  EZ_TreeNode *tnode = EZ_GetListTreeWidgetSelection(treeWidget);
  EZ_Item     *ritem = EZ_GetWorkAreaSelection(resBox);

  if(tnode == NULL || ritem == NULL) return;

  EZ_SetWidgetIntData(resDialogue, 0);
  if(previousSelection != ritem)
    {
      EZ_FreezeWidget(resBox);
      EZ_ConfigureItem(previousSelection, 
                       EZ_LABEL_PIXMAP, NULL,
                       0);
      previousSelection = ritem;
      EZ_ConfigureItem(ritem, 
                       EZ_LABEL_PIXMAP, tickXpm,
                       0);
      EZ_UnFreezeWidget(resBox);
    }
  if(1)
    {
      EZ_TreeNode *pnode=tnode;
      EZ_Item *path[32] ;
      nodeData *nd;
      int  cnt, istart, i=31;

      while(pnode)
        {
          path[i] = EZ_TreeNodeGetItem(pnode);
          i--;
          pnode = EZ_TreeNodeGetParent(pnode);
        }
      i++;
      istart = i;
      cnt = 32-i;
      EZ_FreezeDisplay();
      EZ_SetWidgetIntData(resDialogue, cnt);
      for(i = cnt+1; i < 10; i++)
        {
          EZ_DeActivateWidget(resLabels[i][0]);
          EZ_DeActivateWidget(resLabels[i][1]);
          EZ_DeActivateWidget(resLabels[i][2]);
          EZ_DeActivateWidget(resLabels[i][3]);
        }
      EZ_DeActivateWidget(resLabels[cnt][2]);
      EZ_DeActivateWidget(resLabels[cnt][3]);
      for(i = 0; i < cnt; i++)
        {
          EZ_ActivateWidget(resLabels[i][0]);
          EZ_SetRadioButtonGroupVariableValue(resLabels[i][0], 0);
          nd = (nodeData*)EZ_GetItemPtrData(path[istart+i]);
          EZ_ConfigureWidget(resLabels[i][0], EZ_LABEL_STRING, EZ_QuarkToString(nd->iname), 0);
          EZ_ConfigureWidget(resLabels[i][1], EZ_LABEL_STRING, EZ_QuarkToString(nd->cname), 0);
          EZ_ActivateWidget(resLabels[i][1]);
          EZ_ActivateWidget(resLabels[i][2]);
          EZ_ActivateWidget(resLabels[i][3]);
        }
      EZ_ActivateWidget(resLabels[i][0]);
      EZ_ActivateWidget(resLabels[i][1]);
      EZ_SetRadioButtonGroupVariableValue(resLabels[i][0], 0);
      EZ_ConfigureWidget(resLabels[i][0], EZ_LABEL_STRING, ".", 0);
      EZ_ConfigureWidget(resLabels[i][1], EZ_LABEL_STRING, "*", 0);
      
      updateResourceName();
      EZ_EnableWidgetTree(resDialogue);
      EZ_ThawDisplay();
      EZ_DisplayWidget(resDialogue);
    }
}

/****************************************************************************
 *
 *  Display a warning message like
 *
 *        Lab1
 *        Lab2  (red)
 *        lab3
 *        lab4  (blue)
 */
static void acknowledgeCallback(EZ_Widget *widget, void *data)
{
  EZ_Widget *toplevel = (EZ_Widget *)data;
  EZ_ReleaseGrab();
  EZ_HideWidget(toplevel);
}

static void Warning(char *lab1, char *lab2)
{
  static EZ_Widget    *errToplevel;
  static EZ_Widget    *label1, *label2, *ack;
  EZ_Widget *frame, *tmp;

  if(! EZ_WidgetExist(errToplevel))
    {
      frame = EZ_CreateWidget(EZ_WIDGET_FRAME,       NULL,
			      EZ_TRANSIENT,          True,
			      EZ_ORIENTATION,        EZ_VERTICAL,
			      EZ_FILL_MODE,          EZ_FILL_BOTH,
			      EZ_PADX,               20,
			      EZ_PADY,               10,
			      EZ_IPADY,              10,
			      EZ_SIZE,               0, 0,
			      EZ_LABEL_STRING,       "Message ...",
			      NULL);
      errToplevel = frame;
      
      label1= EZ_CreateWidget(EZ_WIDGET_LABEL,       frame,
			      EZ_LABEL_STRING,       lab1,
			      EZ_TEXT_LINE_LENGTH,   100,
			      EZ_PADY,               0,
			      NULL);

      label2= EZ_CreateWidget(EZ_WIDGET_LABEL,       frame,
			      EZ_FOREGROUND,         "red",
			      EZ_LABEL_STRING,       lab2,
			      EZ_PADY,               0,
			      EZ_TEXT_LINE_LENGTH,   100,
			      NULL);
      
      ack =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
			      EZ_LABEL_STRING,         "OK",
			      EZ_WIDTH,                0,
			      EZ_HEIGHT,               0,
			      EZ_CALLBACK,             acknowledgeCallback, frame,
			      NULL);
    }
  EZ_ConfigureWidget(label1, EZ_LABEL_STRING, lab1, NULL);
  EZ_ConfigureWidget(label2, EZ_LABEL_STRING, lab2, NULL);
  EZ_DisplayWidgetUnderPointer(ack, -10, -10);
  EZ_SetGrab(errToplevel);
  EZ_SetFocusTo(ack);

}
/****************************************************************************/
static void setResourceCb(EZ_Widget *widget, void *data)
{
  EZ_TreeNode *tnode = EZ_GetListTreeWidgetSelection(treeWidget);
  if(tnode)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(tnode);
      nodeData *nd = (nodeData *) EZ_GetItemPtrData(item);
      char msg[1024];
      char *ptr = EZ_GetEntryString(resEntry);
      char *rname = EZ_GetEntryString(resNameEntry);
      Atom   mtype = EZ_RESOURCE_EDITOR_ATOM;
      int    replyId = EZ_RE_SET_RESOURCE+100;      
      EZ_RegisterMessageHandler(mtype,              /* type of msg it handles */
                                replyId,            /* this is the replyId   */
                                data,               /* client data to be passed to handler */
                                (EZ_MessageHandler) msgHandler,  /* the handler     */
                                1,           /* expire in one second   */
                                timeoutCb,   /* what to do when handler expires */
                                (void *)(data? EZ_RE_INSTALL_RESOURCE: EZ_RE_SET_RESOURCE)
                                );  
  
      sprintf(msg, "%lx", nd->id);
      sprintf(msg+20, "%s: %s", rname, ptr);    
      EZ_SendMessage(mtype,msg, 20+strlen(msg+20),data?EZ_RE_INSTALL_RESOURCE:EZ_RE_SET_RESOURCE,
                     currentClient, replyId, senderIsSelf);  
    }
}
/****************************************************************************/
static void setfilename(EZ_Widget *widget, void *data)
{
  char *ptr = EZ_GetEntryString(widget);
  if(ptr && *ptr)
    {
      strcpy(resFileName, ptr);
      EZ_HideWidget(data);
    }
}

static void setSaveFileCb(EZ_Widget *widget, void *data)
{
  static EZ_Widget *dialogue, *file_entry;
  if(dialogue == NULL)
    {
      dialogue = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, NULL,
                                    "FileEntry", "fileEntry",
                                    EZ_LABEL_STRING, "Please Enter a File Name",
                                    EZ_TRANSIENT,    True, 
                                    0);
      file_entry = EZ_CreateWidget(EZ_WIDGET_ENTRY, dialogue,
                                   EZ_CALLBACK, setfilename, dialogue,
                                   0);
    }
  EZ_SetGrab(dialogue);
  EZ_DisplayWidget(dialogue);
  EZ_SetFocusTo(file_entry);
}

/****************************************************************************/
static void saveFileCb(EZ_Widget *widget, void *data)
{
  if(resFileName[0] == 0)
    Warning(" ", "Pleas set save file first");
  else
    {
      FILE *fp = fopen(resFileName, "a");
      if(fp)
        {
          fprintf(fp, "%s: %s\n",EZ_GetEntryString(resNameEntry),
                  EZ_GetEntryString(resEntry));
          fclose(fp);
        }
    }
}
/****************************************************************************/
static XrmDatabase tmpXrmDB;
static void doHighlight(EZ_TreeNode *node)
{
  if(node)
    {
      EZ_TreeNode *parent;
      EZ_Item *item;
      char  *color;
      nodeData *nd;
      XrmQuark clist[128], nlist[128], Q_type[2];
      XrmValue value;
      int nn = 126;
      clist[127] = nlist[127] = NULLQUARK;
      clist[126] = currentResCName; nlist[126] = currentResIName;
      parent = node;
      while(parent)
        {
          item = EZ_TreeNodeGetItem(parent);
          nd = (nodeData *)EZ_GetItemPtrData(item);
          nn--;
          clist[nn] = nd->cname;
          nlist[nn] = nd->iname;
          parent = EZ_TreeNodeGetParent(parent);
        }
      item = EZ_TreeNodeGetItem(node);
      if(XrmQGetResource(tmpXrmDB, (nlist+nn), (clist+nn), Q_type, &value) == True)
        EZ_ConfigureItem(item, 
                         EZ_LABEL_PIXMAP, tickXpm,
                         0);
      else 
        EZ_ConfigureItem(item, 
                         EZ_LABEL_PIXMAP, NULL,
                         0);
    }
}

static void highlightTreeNodes()
{
  char str[1024];
  EZ_TreeNode *node = EZ_GetListTreeWidgetTree(treeWidget);  
  
  sprintf(str, "%s: nothing", EZ_GetEntryString(resNameEntry));
  tmpXrmDB = XrmGetStringDatabase(str);
  EZ_FreezeWidget(treeWidget);
  EZ_TreeTraverseTree(node, doHighlight, NULL);
  EZ_UnFreezeWidget(treeWidget);
  XrmDestroyDatabase(tmpXrmDB);
}

static void doUnhighlight(EZ_TreeNode *node)
{
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      EZ_ConfigureItem(item, 
                       EZ_LABEL_PIXMAP, NULL,
                       0);
    }
}

static void unhighlightTreeNodes()
{
  EZ_TreeNode *node = EZ_GetListTreeWidgetTree(treeWidget);  
  EZ_FreezeWidget(treeWidget);
  EZ_TreeTraverseTree(node, doUnhighlight, NULL);
  EZ_UnFreezeWidget(treeWidget);
}

/****************************************************************************/
static char *helpInfo = "\n\
    Resource Editor For EZWGL\n\
          Version 1.0\n\
   August 16, 1999, Maorong Zou\n\
";

static void showInfo(EZ_Widget *w, void *d)
{
  static EZ_Widget *frame, *label, *btn;
  if(frame == NULL)
    {
      frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                              EZ_TRANSIENT,   1,
                              EZ_LABEL_STRING, " ",
                              EZ_ORIENTATION, EZ_VERTICAL,
                              0);
      label = EZ_CreateWidget(EZ_WIDGET_LABEL,    frame,
                              EZ_LABEL_STRING,    helpInfo,
                              EZ_TEXT_LINE_LENGTH,64,
                              0);
      btn = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                            EZ_SIZE, 0, 0,
                            EZ_LABEL_STRING, "Close",
                            EZ_UNDERLINE,    0,
                            EZ_CALLBACK, hideWidget, frame,
                            0);
    }
  EZ_DisplayWidget(frame);
  EZ_SetFocusTo(btn);
}
/****************************************************************************/
static void nameEntryCb(EZ_Widget *entry, void *data)
{
  if(EZ_EntryIsModified(entry))
    {
      int i, cnt = EZ_GetWidgetIntData(resDialogue);
      for(i = 0; i <= cnt; i++)
        EZ_SetRadioButtonGroupVariableValue(resLabels[i][0], -1);  
      highlightTreeNodes();  
    }
}       

static void valueEntryCb(EZ_Widget *entry, void *data)
{
  setResourceCb(NULL, NULL);
}
/****************************************************************************/
