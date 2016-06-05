/****************** Pixmap Server *************************************/
#include "EZ.h"

#define INSTRUCTION  " \
\n\n  This program demonstrates MESSAGES. \n\n\
It is a pixmap 'server'. You need to start\n 'client'\
to see the demo."


Atom  PIXMAP_ATOM;

void msgHandler(EZ_Message *msg, void *data)
{
  /*
   * the default message handler:
   * if message type is PXIMAP_ATOM, try to create
   * a pixmap from the file supplied, and send the
   * pixmap id and shape id back.
   */
  static EZ_TextProperty  *tprop[2] = {NULL, NULL};
  EZ_TextProperty         *tmpTp;
  char                    buf[1024], *ptr;
  static                  int flag = 0;
  EZ_Widget               *text = (EZ_Widget *)data;

  if(tprop[0] == NULL)
    {
      tprop[0] =  EZ_GetTextProperty(EZ_BACKGROUND, "bisque",  
                                     EZ_FOREGROUND, "blue",
                                     0);
      tprop[1] =  EZ_GetTextProperty(EZ_BACKGROUND, "skyblue1", 
                                     EZ_FOREGROUND, "red",
                                     0);
    }
  if(msg->messageType == PIXMAP_ATOM && msg->messageLength > 0)
    {
      char   reply[1024];
      Pixmap pixmap, shape;
      int    w, h;
      EZ_Pixmap *pptr;
      sprintf(buf, "\n+++From [%lx], Type=[%lx], Len=[%d] Id=[%d]+++\n   %s\n",
              msg->sender, msg->messageType, msg->messageLength,
              msg->replyId,  msg->message);
      EZ_TextInsertStringWithProperty(text, buf, tprop[flag]);
      if((pptr = EZ_CreateLabelPixmapFromXpmFile(msg->message)) == NULL)
        pptr = EZ_CreateLabelPixmapFromImageFile(msg->message);
      if(pptr)
        {
          EZ_GetLabelPixmapInfo(pptr, &pixmap, &shape, &w, &h);
          sprintf(reply, "%d %d %lx %lx", w, h, pixmap, shape);
          EZ_ReplyMessage(msg, reply, strlen(reply));
          sprintf(buf, "   [pixmap=%lx shape=%lx]: replied\n", pixmap,shape);
          EZ_TextInsertStringWithProperty(text, buf, tprop[flag]);                  
          tmpTp =  EZ_GetTextProperty(EZ_LABEL_PIXMAP, pptr, NULL);
          EZ_TextInsertStringWithProperty(text, buf, tmpTp);                  
        }
      else
        {
          sprintf(buf, "  Invalid image. Couldn't create pixmap\n");
          EZ_TextInsertStringWithProperty(text, buf, tprop[flag]);  
        }
      flag = !flag;
    }
  else
    {
      sprintf(buf, "\n---From [%lx], Type=[%lx], Len=[%d] Id=[%d]----\n   %s\n",
              msg->sender, msg->messageType, msg->messageLength,
              msg->replyId,  msg->message); 
      EZ_TextInsertString(text, buf);                
    }
}

main(int ac, char **av)
{
  EZ_Widget *frame, *text;

  EZ_Initialize(ac, av, 0);
  PIXMAP_ATOM =  EZ_GetAtom("PixmapTestAtom");

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "server",           "Server",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_LABEL_STRING,    "Pixmap server",
                              EZ_IPADX,           4,
                              EZ_SIZE,            400, 400,
                              0);
  text =  EZ_CreateWidgetXrm(EZ_WIDGET_TEXT,      frame,
                             "text",              "Text",
                              0);
  EZ_TextInsertString(text, INSTRUCTION);
  EZ_SetDefaultMessageHandler(msgHandler, text);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*************************************************************************/
