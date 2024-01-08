/**********************************************************/
#include "EZ.h"

int decodeFileName(EZ_Widget *, void *, char *, int);
int decodeFileContents(EZ_Widget *, void *, char *, int);
int decodeBG(EZ_Widget *, void *, char *, int);

Atom MY_FILE_NAME_ATOM;
Atom MY_FILE_CONTENTS_ATOM;
Atom MY_BG_ATOM;

int main(int ac, char **av)
{
  EZ_Widget *frame, *textW;
  
  EZ_Initialize(ac,av,0);
  MY_FILE_NAME_ATOM = EZ_GetAtom("MY_FILE_NAME_ATOM");
  MY_FILE_CONTENTS_ATOM = EZ_GetAtom("MY_FILE_CONTENTS_ATOM");
  MY_BG_ATOM = EZ_GetAtom("MY_BG_ATOM");

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,NULL,
                          EZ_LABEL_STRING, "Drag target",
                          EZ_SIZE,          600,400,
                          EZ_FILL_MODE, EZ_FILL_BOTH, 0);
  textW = EZ_CreateWidget(EZ_WIDGET_TEXT, frame, 0);

  EZ_WidgetAddDnDDataDecoder(textW, MY_FILE_CONTENTS_ATOM, 0,
			     decodeFileContents, NULL,
			     NULL, NULL);
  EZ_WidgetAddDnDDataDecoder(textW, MY_FILE_NAME_ATOM, 0,
			     decodeFileName, NULL,
			     NULL, NULL);
  EZ_WidgetAddDnDDataDecoder(textW, MY_BG_ATOM, 0,
			     decodeBG, NULL,
			     NULL, NULL);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

int decodeFileName(EZ_Widget *widget, void *data,
		   char *message, int length)
{
  if(widget)
    {
      if(length > 0)
	{
	  FILE *fp = fopen(message, "r");
	  if(fp)
	    {
	      EZ_TextLoadFile(widget, message);
	      fclose(fp);
	      return(EZ_DND_SUCCESS);
	    }
	}
    }
  return(EZ_DND_FAILURE);
}

int decodeFileContents(EZ_Widget *widget, void *data,
		       char *message, int length)
{
  if(widget)
    {
      if(length > 0)
	{
	  EZ_FreezeWidget(widget);
	  EZ_TextClear(widget);
	  EZ_TextInsertString(widget, message);
	  EZ_UnFreezeWidget(widget);
	  return(EZ_DND_SUCCESS);
	}
    }
  return(EZ_DND_FAILURE);
}

int decodeBG(EZ_Widget *widget, void *data,
	     char *message, int length)
{
  if(widget)
    {
      /* the message is a correct color specification */
      EZ_ConfigureWidget(widget, EZ_FOREGROUND, message, 0);
      return(EZ_DND_SUCCESS);
    }
  return(EZ_DND_FAILURE);
}

/**********************************************************/
