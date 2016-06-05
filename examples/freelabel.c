/***************************** Example **************************/
/*
 *  Test FreeLabel
 */

#include "EZ.h" 

static void label_callback(EZ_Widget *button);
static char *label= "I'm a free label. My position is not assigned\
by my parent. Move me by using Button 1 press-drag-release.\
 Double click on me to make me bounce";

int _test_ = 0;
EZ_Widget *frame;
main(int argc, char **argv)
{
  EZ_Widget *tmp;
  int i, j, k,m,n;

  if(argc > 1) _test_ = 1;
  EZ_Initialize(argc,argv,0);       /* initialize EZWGL    */
  srand(getpid());
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame,
        EZ_BORDER_TYPE, EZ_BORDER_RAISED, EZ_BORDER_WIDTH, 2,
        EZ_BACKGROUND, "bisque",		     
        EZ_WIDTH,400, EZ_HEIGHT, 400, 0);

  tmp = EZ_CreateFreeLabel(frame, label);
  EZ_ConfigureWidget(tmp, 
		     EZ_CALL_BACK, label_callback, NULL,
		     EZ_BORDER_TYPE, EZ_BORDER_RIDGE, EZ_BORDER_WIDTH, 2,
		     EZ_TEXT_LINE_LENGTH, 40,
		     EZ_FOREGROUND, "red",
		     EZ_BACKGROUND,"gray",
		     EZ_FONT_NAME,
		     "-Adobe-Times-Bold-R-Normal--*-180-*-*-*-*-*-*",
		     EZ_JUSTIFICATION, EZ_CENTER,
		     EZ_X, ( (rand()>>5) % 300),
		     EZ_Y, ( (rand()>>5) % 300), 0);
  EZ_DisplayWidget(frame); 
  EZ_EventMainLoop();
}

static void label_callback(EZ_Widget *widget)
{
  int i, x, y, h, H, speed = 0;

  EZ_GetWidgetPosition(widget, &x,&y);
  h = EZ_GetWidgetHeight(widget);
  H = EZ_GetWidgetHeight(frame) -2;

  for(i = y; i <= H - h; i += speed, speed++)
    EZ_ConfigureWidget(widget, EZ_X,x, EZ_Y, i, 0);
  speed++;
  for(; i > y; i -= speed, speed--)
    EZ_ConfigureWidget(widget, EZ_X,x, EZ_Y, i, 0);
}
/***************************** Example **************************/
