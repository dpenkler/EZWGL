/***************** Example 2 ********************************/

#include "EZ.h"                                     /* the header file     */
#include <string.h>
#include <unistd.h>

static void  cleanExit(EZ_Widget *, void *);        /* quit btn callback   */
static void  button_callback(EZ_Widget *, void *);  /* button callback     */
static void  randomize(EZ_Widget *, void *);        /* scramble the puzzle */

static EZ_Widget *buttons[4][4];                    /* the buttons         */
static int width = 39, height = 33;                 /* size of the buttons */
static int emptyx = 3, emptyy = 3;                  /* the empty slot      */
static int hpad = 1;


int main(int ac, char **av)
{
  int        i,j, k;
  char       str[4], *fname = NULL;
  EZ_Widget  *toplevel, *frame, *label, *tmp, *tmp1;
  Pixmap     pixmap;
  unsigned int pw, ph;
  
  EZ_Initialize(ac,av,0);                                 /* Initialize EZWGL   */
  i = 1; 
  while(i < ac) 
    {
      if(!strcmp(av[i], "-f") && i+1 < ac) fname = av[i+1];
      i++;
    }
  if(fname == NULL ) fprintf(stderr, "Usage %s -f image_file\n",av[0]);
                                                          /* the toplevel frame */
  toplevel = EZ_CreateWidget(EZ_WIDGET_FRAME,NULL,
                             EZ_STACKING, EZ_VERTICAL,    /* vert. orientation  */
                             0);

  label = EZ_CreateWidget(EZ_WIDGET_LABEL, toplevel,
                          EZ_LABEL_STRING, "The 15 Puzzle",
                          EZ_FOREGROUND, "blue",
                          EZ_FONT_NAME,
                            "-Adobe-Times-Bold-R-Normal--*-180-*-*-*-*-*-*", 
                          0);

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, toplevel,         /* the button box  */
                          EZ_PADX,0, EZ_PADY,0,              /* no padding      */
                          EZ_BORDER_TYPE,EZ_BORDER_SUNKEN,   /* a sunken border */
                          EZ_BORDER_WIDTH, 2,                /* of width 2      */
                          EZ_WIDTH, 0, EZ_HEIGHT, 0,         /* always keep it  */
                          0);                                /* at its min size */

  tmp = EZ_CreateWidget(EZ_WIDGET_FRAME, toplevel,
			EZ_PADX, 10, EZ_PADY, 10,
                        0);                /* a frame to hold two handy buttons  */
  tmp1 = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmp,         /* a quit button  */
                         EZ_LABEL_STRING, "Quit",
                         0);
  EZ_AddWidgetCallBack(tmp1, EZ_CALLBACK, cleanExit, NULL, 0);

  tmp1 = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmp,    /* and a new game btn */    
                         EZ_LABEL_STRING, "Scramble",
                         0);
  EZ_AddWidgetCallBack(tmp1, EZ_CALLBACK, randomize, NULL,0);

  if(fname && EZ_CreateXPixmapFromImageFile(fname,&pw, &ph, &pixmap))
    {
      width = pw/4 + 2*hpad; 
      height = ph/4 + 2*hpad;
    }
  else  pixmap = (Pixmap)NULL;
  
  for(i = 0; i < 4; i++)                       /* create buttons for the puzzle */
    for(j = 0; j < 4; j++)                     /* 4 rows, 4 columns,    1 btns  */
      {
        if(i + j != 6) /* ignore the last one */
          {
            k = 4 * i + j;
            sprintf(str,"%d",k+1);                            /* btn at [i,j] */
            tmp = buttons[i][j] = 
                  EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                  EZ_LABEL_STRING,str, 
                                  EZ_X, 3 + j * width, /* and a perminent size     */
                                  EZ_Y, 3 + i * height,
                                  EZ_WIDTH, width, EZ_HEIGHT, height, 
                                  EZ_PROPAGATE, False,
				  EZ_BORDER_WIDTH, 1,
                                  EZ_CALLBACK, button_callback, NULL, /* unique callback */
				  EZ_HIGHLIGHT_PAD, 0,
                                  EZ_HIGHLIGHT_BG, "red3",
                                  0);
            EZ_SetWidgetIntData(tmp, k);    /* K encodes its initial location   */

	    if(pixmap)
	      EZ_ConfigureWidget(tmp, 
				 EZ_X_PIXMAP, pixmap, j*(width-2*hpad),i*(height-2*hpad), 
				 width-2*hpad, height-2*hpad,  0);
	  }
      }
  srand(getpid()); 
  EZ_DisplayWidget(toplevel); 
  EZ_EventMainLoop();
}

static void button_callback(EZ_Widget *widget, void *data)
{
  int tmp, x, y;
  
  tmp = EZ_GetWidgetIntData(widget);                       /* cur location  */
  x = tmp >> 2;                            /* at the yth row, the x' column */
  y = tmp & 0x3;
  
  if(abs(emptyx - x) + abs(emptyy - y) == 1)    /* next to the empty square */
    {                                        /*  swap with the empty square */
      int tx = emptyx, ty = emptyy;
      emptyx = x; emptyy = y;                       /* the new empty square */
      EZ_SetWidgetIntData(widget, 4 * tx + ty);     /* and my new location  */
      EZ_SetWidgetPosition(widget, 3 + ty * width, 3 + tx * height);
    }
}

static void randomize(EZ_Widget *widget, void *data)
{                                          /* just make a few random moves */
  int i, x, y;
  for(i = 0; i < 256; i++)
    {
      x = (rand()>> 5) % 4;
      y = (rand()>> 5) % 4;
      if((x + y != 6)) button_callback(buttons[x][y], NULL);
    }
}
static void  cleanExit(EZ_Widget *w, void *data)
{
  EZ_Shutdown();
  exit(0);
}
/***************** Example 2 ********************************/
