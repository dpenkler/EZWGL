/*********************** Client *******************************************/
#include "EZ.h"

Atom  PIXMAP_ATOM;

void msgHandler(EZ_Message *msg, void *data)
{
  EZ_Widget        *label = (EZ_Widget *)data;
  if(msg->messageLength > 0)
    {
      Pixmap pixmap, shape;
      int    w, h;
      if(sscanf(msg->message, "%d %d %lx %lx", &w, &h, &pixmap, &shape) == 4 && w > 0)
        {
          EZ_Pixmap *ptr = EZ_CreateLabelPixmapFromXPixmap(pixmap, shape, 0, 0, w,h, 1);
          if(ptr) EZ_ConfigureWidget((EZ_Widget *)data, EZ_LABEL_PIXMAP, ptr, 0);
          /* delete this handler */
          EZ_DeleteMessageHandler(msg->messageType, msg->messageId, data, 
                                  (EZ_MessageHandler) msgHandler);
        }
    }
}

static void timeoutCb(void *label, void *data)
{
  char str[256];
  sprintf(str, "message handler for msgId \n \t '%d'\n expired", (int)data);
  EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, EZ_LABEL_PIXMAP, NULL, 0);
}

void ltreeCB(EZ_Widget *tree, void *data)
{
  EZ_TreeNode  *node = EZ_GetListTreeWidgetSelection(tree);
  EZ_Item      *item;
  static       int  msgId = 1;

  if(node && (item = EZ_TreeNodeGetItem(node)))
    {
      char *path = EZ_GetDirTreeNodeFullPath(node);    
      /* add a message handler */
      EZ_RegisterMessageHandler(PIXMAP_ATOM, /* type of msg it handles */
                                msgId+1,     /* the msgId it handles   */
                                data,        /* client data to be passed to handler */
                                (EZ_MessageHandler) msgHandler,  /* the handler            */
                                1,           /* expire in one second   */
                                timeoutCb,   /* what to do when handler expires */
                                (void *)(msgId+1) /* client data to be passed to timeoutCb */
                                );
      /* broadcast a message to find a server */
      EZ_BroadcastMessage(PIXMAP_ATOM,  /* msg type */
                          path,         /* the message */
                          strlen(path), /* length of message */
                          msgId,        /* this message Id   */
                          msgId+1       /* ask recipient to reply using this id */
                          );
      msgId += 2;
    }
}

main(int ac, char **av)
{
  EZ_Widget *frame, *ltree, *label, *phandle;
  EZ_TreeNode *root;

  EZ_Initialize(ac, av, 0);
  PIXMAP_ATOM =  EZ_GetAtom("PixmapTestAtom");

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "server",           "Server",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_LABEL_STRING,    "Pixmap client",
                              EZ_IPADX,           4,
                              EZ_SIZE,            800, 400,
                              0);
  ltree =  EZ_CreateWidgetXrm(EZ_WIDGET_TREE,      frame,
                              "tree",              "Tree",
                              EZ_WIDTH,            300,
                              0);
  phandle = EZ_CreateWidget(EZ_WIDGET_PANE_HANDLE, frame, 
                            0);

  label = EZ_CreateWidgetXrm(EZ_WIDGET_LABEL,      frame,
                             "label",              "Label",
                             EZ_FOREGROUND,        "red",
                             EZ_FONT_NAME,  "-Adobe-Times-Bold-I-Normal--*-240-*-*-*-*-*-*",
                             0);
  EZ_AddWidgetCallBack(ltree, EZ_CALLBACK, ltreeCB, label, 0);

  EZ_GlobHiddenFiles(True);
  root = EZ_CreateDirTree(NULL, "./*", NULL, 0);  /* */
  EZ_SetListTreeWidgetTree(ltree, root); 

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*************************************************************************/



