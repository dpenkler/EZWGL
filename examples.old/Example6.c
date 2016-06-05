/************************** Example 6 ***********************/
#include "EZ.h"                     /* the header file      */

EZ_Widget *Frame, *LabelA, *LabelB;
Display   *display;
Visual    *visual;
int       screenNum, depth;

static void eventHandler(EZ_Widget *, void *, int, XEvent *);

main(int argc, char **argv)
{
  EZ_Initialize(argc,argv,0);    /* initialize  EZWGL        */
  EZ_DisableImageDithering();    /* turn off image dithering */
  EZ_GetDisplayInfo(&display,    /* get display info         */
		    &screenNum, &visual, &depth);

  Frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(Frame, EZ_ORIENTATION, EZ_VERTICAL,
		     EZ_PADX, 0, EZ_PADY,0, 0);

  LabelA = EZ_CreateLabel(Frame, NULL);
  EZ_ConfigureWidget(LabelA, EZ_WIDTH,0, EZ_HEIGHT, 0,
		     EZ_PADX, 0, EZ_PADY,0,
		     EZ_IMAGE_FILE, "guilin.gif", 0);
  EZ_AddEventHandler(LabelA, eventHandler, NULL, 0);

  LabelB = EZ_CreateLabel(Frame, NULL);
  EZ_ConfigureWidget(LabelB, EZ_WIDTH,200, EZ_HEIGHT, 40, 
		     EZ_PROPAGATE, 0,  /* don't leave this out !!! */
		     EZ_LABEL_POSITION, EZ_LEFT, 0);

  EZ_DisplayWidget(Frame);

  EZ_EventMainLoop();
}
/***********************************************************************/
static read_pixel(Window win, int x,int y)
{
  XImage *image;

  image = XGetImage(display, win, x, y, 1, 1, (1<< depth) - 1, ZPixmap);
  if(image != (XImage *)NULL)
    {
      unsigned long pixel = XGetPixel(image,0,0);
      int R,G,B;
      char buf[128], bufA[32], bufB[32];
      EZ_PixelValue2RGB(pixel, &R,&G,&B);
      sprintf(buf,"Pixel: x=%d, y=%d\nColor: %d %d %d",x,y,R,G,B);
      sprintf(bufA,"#%02x%02x%02x", R,G,B);
      sprintf(bufB,"#%02x%02x%02x", 255-R,255-G,255-B);
      EZ_ConfigureWidget(LabelB, EZ_LABEL_STRING, buf,
			 EZ_BACKGROUND, bufA, 
			 EZ_FOREGROUND, bufB, 0);
      XDestroyImage(image);
    }
}

void eventHandler(EZ_Widget *widget, void *data, int etype, XEvent *event)
{
  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == Button1)
	{
	  int x = event->xbutton.x;
	  int y = event->xbutton.y;
	  Window win = event->xbutton.window;
	  read_pixel(win, x,y);
	}
      break;
    case MotionNotify:
      if(event->xmotion.state & Button1Mask)
	{
	  int x = event->xmotion.x;
	  int y = event->xmotion.y;
	  Window win = event->xmotion.window;
	  read_pixel(win, x,y);	  
	}
      break;
    default:
      break;
    }
}
/************************** Example 6 ***********************/
