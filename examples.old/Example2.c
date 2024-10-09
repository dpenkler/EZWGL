/***************** Example 2 ********************************/
#include <unistd.h>
#include "EZ.h"                                     /* the header file     */

static void  cleanExit(EZ_Widget *, void *);        /* quit btn callback   */
static void  button_callback(EZ_Widget *, void *);  /* button callback     */
static void  randomize(EZ_Widget *, void *);        /* scramble the puzzle */

static EZ_Widget *buttons[4][4];                    /* the buttons         */
static int width = 39, height = 33;                 /* size of the buttons */
static int emptyx = 3, emptyy = 3;                  /* the empty slot      */

int main(int ac, char **av)
{
  int        i,j, k;
  char       str[4];
  EZ_Widget  *toplevel, *frame, *label, *tmp, *tmp1;
  
  EZ_Initialize(ac,av,0);                                 /* Initialize EZWGL   */
  toplevel = EZ_CreateFrame(NULL,NULL);                   /* the toplevel frame */
  EZ_ConfigureWidget(toplevel,                            /* vert. orientation  */
                     EZ_STACKING, EZ_VERTICAL, 0);

  label = EZ_CreateLabel(toplevel, "The 15 Puzzle");                 /* a label */
  EZ_ConfigureWidget(label, EZ_FOREGROUND, "blue",     /* font and clr of label */
         EZ_FONT_NAME, "-Adobe-Times-Bold-R-Normal--*-180-*-*-*-*-*-*", 0);

  frame = EZ_CreateFrame(toplevel, NULL);                    /* the button box  */
  EZ_ConfigureWidget(frame, EZ_PADX,0, EZ_PADY,0,            /* no padding      */
                     EZ_BORDER_TYPE,EZ_BORDER_SUNKEN,        /* a sunken border */
                     EZ_BORDER_WIDTH, 2,                     /* of width 2      */
                     EZ_WIDTH, 0, EZ_HEIGHT, 0, 0);          /* always keep it  */
                                                             /* at its min size */
  tmp = EZ_CreateFrame(toplevel, NULL);   /* a frame to hold two handy buttons  */
  tmp1 = EZ_CreateButton(tmp, "Quit",0);                      /* a quit button  */
  EZ_AddWidgetCallBack(tmp1, EZ_CALLBACK, cleanExit, NULL, 0);
  tmp1 = EZ_CreateButton(tmp, "Scramble",0);              /* and a new game btn */
  EZ_AddWidgetCallBack(tmp1, EZ_CALLBACK, randomize, NULL,0);

  for(i = 0; i < 4; i++)                       /* create buttons for the puzzle */
    for(j = 0; j < 4; j++)                     /* 4 rows, 4 columns,    1 btns  */
      {
        if(i + j != 6) /* ignore the last one */
          {
            k = 4 * i + j;
            sprintf(str,"%d",k+1);
            tmp = buttons[i][j] = EZ_CreateButton(frame,str,-1);/* btn at [i,j] */
            EZ_SetWidgetIntData(tmp, k);    /* K encodes its initial location   */
            EZ_ConfigureWidget(tmp,         /* we need to set the init location */
			       EZ_X, 3 + j * width, /* and a perminent size     */
			       EZ_Y, 3 + i * height,
                               EZ_WIDTH, width, EZ_HEIGHT, height, 
                               EZ_PROPAGATE, False,
                               EZ_CALLBACK, button_callback, NULL, /* unique callback */
                               0);
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
