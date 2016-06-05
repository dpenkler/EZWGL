#include "EZ.h"


main(int ac, char **av)
{
  EZ_Widget *frame, *flistbox, *frame5, *btn, *tmp;
  char       str[256];
  FILE       *fp;
  EZ_Item    *items[10];
  int i, j, cnt=1;

  EZ_Initialize(ac,av,0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frames",           "frames",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_IPADX,           4,
                              EZ_SIZE,            400, 300,
                              0);

  flistbox = EZ_CreateWidget(EZ_WIDGET_FANCY_LIST_BOX, frame,
			     EZ_FANCY_LIST_BOX_COLUMNS, 6,
			     EZ_IPADY, 0,
			     EZ_IPADX, 0,
			     EZ_PADY,  1,
			     EZ_BORDER_WIDTH, 2,
			     EZ_FANCY_LIST_BOX_TYPE, 1, 
			     EZ_SELECTION_FOREGROUND, "blue",
			     EZ_SELECTION_BACKGROUND, "yellow",
                             EZ_ROW_BG,  1, "white", "bisque",
			     0);			     

      /* a header */
  items[0] = EZ_CreateLabelItem("File/Dir Name", NULL);
  items[1] = EZ_CreateLabelItem("User Name", NULL);
  items[2] = EZ_CreateLabelItem("Group Name", NULL);
  items[3] = EZ_CreateLabelItem("File Size ", NULL);
  items[4] = EZ_CreateLabelItem("Permissions", NULL);
  items[5] = EZ_CreateLabelItem("Count", NULL);
  for(i = 0; i < 5; i++)
    {
      switch(i)
        {
        case 0:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "red",0); break;
        case 1:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "blue",0); break;
        case 2:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "yellow",0); break;
        case 3:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "cyan",0); break;
        case 4:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "magenta",0); break;
        default: break;
        }
      EZ_ConfigureItem(items[i], EZ_BORDER_WIDTH,1,EZ_BORDER_TYPE, EZ_BORDER_TB_UP,
                       EZ_BACKGROUND, "bisque2", EZ_PADY, 2,
                       EZ_BORDER_WIDTH, 1,
                       EZ_LABEL_POSITION, EZ_LEFT, 0);
    }
  EZ_SetFancyListBoxHeader(flistbox, items, 6);
  for(i =0; i < 100; i++)
    {
      sprintf(str, "Row %d", i);
      items[0] = EZ_CreateLabelItem(str, NULL);
      items[1] = EZ_CreateLabelItem("some", NULL);
      items[2] = EZ_CreateLabelItem("thingy", NULL);
      items[3] = EZ_CreateLabelItem("col 3", NULL);
      items[4] = EZ_CreateLabelItem("more", NULL);
      items[5] = EZ_CreateLabelItem("stuff", NULL);
      EZ_FancyListBoxInsertRow(flistbox, items, 6, 
                               EZ_GetFancyListBoxNumberOfRows(flistbox)
                               ); /* at the end */      
    }
  EZ_DisplayWidget(frame);
  /* add 5000 rows */
  EZ_FreezeWidget(flistbox);
  for(; i < 5100; i++)
    {
      sprintf(str, "Row %d", i);
      items[0] = EZ_CreateLabelItem(str, NULL);
      items[1] = EZ_CreateLabelItem("some", NULL);
      items[2] = EZ_CreateLabelItem("thingy", NULL);
      items[3] = EZ_CreateLabelItem("col 3", NULL);
      items[4] = EZ_CreateItem(EZ_LABEL_ITEM,
                               EZ_LABEL_STRING, "more",
                               EZ_PIXMAP_FILE, "dog3.xpm",
                               NULL);
      items[5] = EZ_CreateLabelItem("stuff", NULL);
      EZ_FancyListBoxInsertRow(flistbox, items, 6, 
                               EZ_GetFancyListBoxNumberOfRows(flistbox)
                               ); /* at the end */      
    }
  EZ_UnFreezeWidget(flistbox);

  EZ_EventMainLoop();
}
