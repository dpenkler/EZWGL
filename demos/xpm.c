
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "EZ.h"

main(int ac, char **av)
{
  EZ_Widget     *wa;
  EZ_Item       *items[1024];
  int           i, count = 0;
  DIR           *dirptr;
  struct dirent *dent;
  char          ezlibdir[1024];
  char          *dirName;

  EZ_Initialize(ac, av, 0);
  wa = EZ_CreateWidget(EZ_WIDGET_WORK_AREA, NULL,
		       EZ_WIDTH,            600,
		       EZ_HEIGHT,           500,
		       EZ_ITEM_MOVABLE,     False,
		       NULL);

  if(ac < 2)  fprintf(stderr, "Usage: %s dir dir ... \n", av[0]);

  strcpy(ezlibdir, "EZ_LIBDIR=");
  for(i = 1; i < ac; i++)
    {
      strcat(ezlibdir, ":");
      strcat(ezlibdir, av[i]);
    }
  putenv(ezlibdir);

  if(ac < 2) dirName = "./"; else dirName=av[1];
  i = 1;

  do
    {
      if( (dirptr = opendir(dirName)) != NULL && count < 1024)
	{
	  while( (dent = readdir(dirptr)) != NULL && count < 1024)
	    {
	      char   *d_name = dent->d_name;
	      int    len;

	      if(d_name[0] != '.')
		{
		  EZ_Bitmap *bitmap = EZ_CreateLabelPixmapFromImageFile(d_name);
		  if(bitmap)
		    {
		      items[count] = EZ_CreateItem(EZ_LABEL_ITEM,
						   EZ_LABEL_STRING, d_name,
						   EZ_LABEL_PIXMAP, bitmap,
                                                   EZ_PIXMAP_FILE, d_name,
						   EZ_ORIENTATION,  EZ_VERTICAL, 
						   NULL);
		      count++;
		    }
		}
	    }
	  closedir(dirptr);
	}
      i++; if(i < ac) dirName=av[i];
    } while(i < ac);

  EZ_WorkAreaInsertItems(wa, items, count);
  EZ_DisplayWidget(wa);
  EZ_EventMainLoop();
}

