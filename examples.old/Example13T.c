/******************* Example 15T ***************************************/
#include "EZ.h"

int decodeFileName(EZ_Widget *, void *, char *, int);
int decodeFileContents(EZ_Widget *, void *, char *, int);

Atom MY_FILE_NAME_ATOM;
Atom MY_FILE_CONTENTS_ATOM;

int main(int ac, char **av)
{
  EZ_Widget *frame, *textW;
  
  EZ_Initialize(ac,av,0);
  MY_FILE_NAME_ATOM = EZ_GetAtom("MY_FILE_NAME_ATOM");
  MY_FILE_CONTENTS_ATOM = EZ_GetAtom("MY_FILE_CONTENTS_ATOM");

  frame = EZ_CreateFrame(NULL, "Drag target");
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH, 0);
  textW = EZ_CreateTextWidget(frame, 1,1,1);

  EZ_WidgetAddDnDDataDecoder(textW, MY_FILE_CONTENTS_ATOM, 0,
			     decodeFileContents, NULL,
			     NULL, NULL);
  EZ_WidgetAddDnDDataDecoder(textW, MY_FILE_NAME_ATOM, 0,
			     decodeFileName, NULL,
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
/******************* Example 15T ***************************************/
