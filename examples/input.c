/******************* ExampleInput ***************************************/
#include <unistd.h>

#include "EZ.h"
static void readStdin(EZ_Input *id, void *data, int fd, int mask)
{
  EZ_Widget *tw = (EZ_Widget *)data;
  char buf[1024];
  int n;
  if(mask & EZ_READABLE_MASK)
    {
      n = read(fd, buf, 1023);
      if(n > 0)
	{
	  buf[n] = 0;
	  EZ_TextEndOfBuffer(tw);
	  EZ_TextInsertString(tw,buf);
	}
    }
}

main(int ac, char **av)
{
  EZ_Widget *frame, *textW;
  
  EZ_Initialize(ac,av,0);

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                          EZ_LABEL_STRING, "Test EZ_AddInput",
                          EZ_FILL_MODE,    EZ_FILL_BOTH,
                          EZ_SIZE,         300, 300, 
                          0);
  textW = EZ_CreateWidget(EZ_WIDGET_TEXT, frame,
                          EZ_TEXT_BACKGROUND, "white",
                          EZ_BORDER_WIDTH,     1,
                          0);
  EZ_TextInsertString(textW, "Enter someting in the xterm window\n and it will appear here\n");

  EZ_DisplayWidget(frame);
  EZ_AddInput(0, EZ_READABLE_MASK, readStdin, textW);
  EZ_EventMainLoop();
}
/******************* ExampleInput ***************************************/
