/**************************** ExampleFLB ******************************/
#include "EZ.h"

main(int ac, char **av)
{
  char       str[256];
  FILE       *fp;
  EZ_Item    *items[10];
  EZ_Widget  *flistbox;
  int i;
  
  EZ_Initialize(ac, av, 0);
  
  flistbox = EZ_CreateWidget(EZ_WIDGET_FANCY_LIST_BOX, NULL,
			     EZ_FANCY_LIST_BOX_COLUMNS, 5,
			     EZ_FANCY_LIST_BOX_TYPE, 1,
			     EZ_WIDTH, 400, EZ_HEIGHT, 500,
			     EZ_IPADY, 2, 
			     EZ_SELECTION_FOREGROUND, "black",
			     0);			     
  fp = popen("ls -alg | awk '{print $9 \" \" $3 \" \" $4 \" \" $5 \" \" $1}'", "r");
  if(fp)
    {
      char fields[6][32];
      while(fgets(str, 255, fp) != NULL)
        {
          if(sscanf(str, "%s %s %s %s %s",fields[0],fields[1],fields[2],fields[3],fields[4]) == 5)
            {
              for(i = 0; i < 5; i++)
                {
                  items[i] = EZ_CreateLabelItem(fields[i], NULL);
		  EZ_ConfigureItem(items[i], EZ_BORDER_TYPE, EZ_BORDER_RAISED, EZ_BORDER_WIDTH, 1,0);

                  switch(i)
                    {
                    case 0:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "red",EZ_PADX, 4,0); 
                      if(fields[4][0] == 'd') EZ_ConfigureItem(items[i], EZ_PIXMAP_FILE, "folder.xpm", 0);
                      else  EZ_ConfigureItem(items[i], EZ_PIXMAP_FILE, "file.xpm",0);
                      break;
                    case 1:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "blue",0); break;
                    case 2:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "yellow",0); break;
                    case 3:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "cyan",0); break;
                    case 4:  EZ_ConfigureItem(items[i], EZ_FOREGROUND, "magenta",0); break;
                    default: break;
                    }
                }
              EZ_FancyListBoxInsertRow(flistbox, items, 5, -1); /* at the end */
            }
        }
      /* a header */
      items[0] = EZ_CreateLabelItem("File/Dir Name", NULL);
      items[1] = EZ_CreateLabelItem("User Name", NULL);
      items[2] = EZ_CreateLabelItem("Group Name", NULL);
      items[3] = EZ_CreateLabelItem("File Size ", NULL);
      items[4] = EZ_CreateLabelItem("Permissions", NULL);
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
          EZ_ConfigureItem(items[i], EZ_BORDER_WIDTH,2,EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                           EZ_BACKGROUND, "bisque2", EZ_PADY, 2,
                           EZ_LABEL_POSITION, EZ_LEFT, 0);
         }
      EZ_SetFancyListBoxHeader(flistbox, items, 5);
    }
  pclose(fp);
  EZ_DisplayWidget(flistbox);
  EZ_EventMainLoop();
}




