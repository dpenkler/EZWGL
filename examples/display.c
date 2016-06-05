#include <X11/Xlib.h>

main(int ac, char **av)
{
  Display *dpy = NULL;
  int i;
  char *arg, *dpy_name = NULL;

  for(i = 1; i < ac; i++)
    {
      arg = av[i];
      if(!strcmp(arg,"-display"))
        {
          if(++i < ac) dpy_name = av[i];
          else --i;
        }  
    }

  if((dpy = XOpenDisplay(dpy_name)) == NULL)
    exit(1);
  XCloseDisplay(dpy);
  exit(0);
}
