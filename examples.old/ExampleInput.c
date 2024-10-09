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

int main(int ac, char **av)
{
  EZ_Widget *frame, *textW;
  
  EZ_Initialize(ac,av,0);

  frame = EZ_CreateFrame(NULL, "Test EZ_AddInput");
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH, EZ_WIDTH, 200, EZ_HEIGHT, 300, 0);
  textW = EZ_CreateTextWidget(frame, 1,1,1);

  EZ_DisplayWidget(frame);
  EZ_AddInput(0, EZ_READABLE_MASK, readStdin, textW);
  EZ_EventMainLoop();
}
/******************* ExampleInput ***************************************/
