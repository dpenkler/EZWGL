/************************** resource editor ****************************/
#include "EZ.h"

static void readStdin(EZ_Input *id, void *data, int fd, int mask)
{
  EZ_Widget *textW = (EZ_Widget *)data;
  char buf[8196];
  int n;
  if(mask & EZ_READABLE_MASK)
    {
      n = read(fd, buf, 8192);
      if(n > 0)
	{
	  buf[n] = 0;
          EZ_FreezeWidget(textW);
          EZ_TextInsertString(textW, buf);
          EZ_UnFreezeWidget(textW);
	}
    }
}

static void acknowledgeCallback(EZ_Widget *widget, void *data)
{
  EZ_ReleaseGrab();
  EZ_DestroyWidget((EZ_Widget *)data);
  EZ_Shutdown();
  exit(0);
}

/**********************************************************************************/

main(int ac, char **av)
{
  EZ_Widget           *frame, *tmp, *ack;

  if(EZ_InitializeXrm("Warning", NULL, 0, &ac, av, NULL, 0) >= 0)
    {

      frame = EZ_CreateWidget(EZ_WIDGET_FRAME,       NULL,
                              EZ_TRANSIENT,          True,
                              EZ_ORIENTATION,        EZ_VERTICAL,
                              EZ_FILL_MODE,          EZ_FILL_BOTH,
                              EZ_PADX,               20,
                              EZ_PADY,               10,
                              EZ_IPADY,              10,
                              EZ_SIZE,               0, 0,
                              EZ_FONT_NAME,  "-Adobe-Helvetica-Bold-R-Normal--*-180-*-*-*-*-*-*",
                              EZ_LABEL_STRING,       "Warning",
                              EZ_FOREGROUND, "red",
                              EZ_SIZE,               640, 400,
                              NULL);
      
      tmp =   EZ_CreateWidget(EZ_WIDGET_TEXT,       frame,
                              EZ_FONT_NAME,      "-Adobe-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*",
                              EZ_TEXT_WIDGET_EDITABLE,False,
                              EZ_WRAP_TEXT,           True,
                              NULL);
  
      EZ_AddInput(0, EZ_READABLE_MASK, readStdin, tmp);

      ack =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                              EZ_LABEL_STRING,         "Acknowledge",
                              EZ_WIDTH,                0,
                              EZ_HEIGHT,               0,
                              EZ_UNDERLINE,            0,
                              EZ_CALLBACK,             acknowledgeCallback, frame,
                              NULL);


      EZ_DisplayWidget(frame);
      EZ_SetFocusTo(ack);
      EZ_EventMainLoop();
    }
  else
    {
      char buf[1024];
      while(gets(buf)) puts(buf);
    }
}
