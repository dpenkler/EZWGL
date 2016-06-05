/*********************************************************************
 *
 *   Use XImage with EZWGL.
 *  
 *  This program is just an example to show you how to use XImage
 *  in EZWGL. 
 */

#include "EZ.h"

Display     *dpy;
Visual      *visual;
int         depth;
int         bits_per_pixel;
int         screen;
EZ_Widget   *label;
Window      window;
XImage      *ximage;
char        *imageData;
int         bytes_per_line;
GC          gc;

/***************************************************************/
static void make_random_image( void );

/***************************************************************
 *
 * Macros convert r g b values to pixel values on
 * TrueColor visuals, (r g b are shorts) and vice versa
 *
 *   For TrueColor and DirectColor Visuals.
 */
#define  RGB2Pixel(r,g,b)                                      \
( ((( (r) >> (_RedBits) ) << (_RedShift)) & (_RedMask))  |     \
  ((( (g) >> (_GreenBits)) << (_GreenShift)) & (_GreenMask)) | \
  ((( (b) >> (_BlueBits)) << (_BlueShift)) & (_BlueMask)))

/*
 * Use table look up.
 */
#define  RGB2PixelLookUp(r,g,b) \
(EZ_RedPixelValues[(int)(r)] | EZ_GreenPixelValues[(int)(g)] | EZ_BluePixelValues[(int)(b)])

extern int               _RedMask;          /* Masks, may differ from   */
extern int               _GreenMask;        /* the corresponding part   */
extern int               _BlueMask;         /* in EZ_Visual->***mask    */

extern int               _RedShift;         /* number of shifts needed  */
extern int               _GreenShift;       /* to pump R G B to their   */
extern int               _BlueShift;        /* position in pixelvalues  */

extern int               _RedBits;          /* number of insignificant  */
extern int               _GreenBits;        /* bits in R G B intensity  */
extern int               _BlueBits;         /* settings. Need to clear  */

/***************************************************************/


main(int argc, char **argv)
{
  EZ_Widget *frame;

  EZ_Initialize(argc,argv,0);       /* initialize EZWGL    */

  EZ_GetDisplayInfo(&dpy, &screen, &visual, &depth);

  /*
  if(visual->class != TrueColor)
    {
      fprintf(stderr, "This example works only on TrueColor visuals\n");
      fprintf(stderr, "Try run it with the option '-visual TrueColor'\n");
      exit(1);
    }
    */
  /* see if we have 24 packed bits */
  bits_per_pixel = depth;
  if(depth == 24)
    {
      XPixmapFormatValues *pmf;
      int i, n;
      pmf = XListPixmapFormats(dpy, &n);
      if(pmf) 
	{
	  for(i = 0; i < n; i++) 
	    {
	      if(pmf[i].depth == 24)
		{ bits_per_pixel =  pmf[i].bits_per_pixel; break; }
	    }
	  XFree ((char *) pmf);
	}
    }
  if(bits_per_pixel == 24)
    {
      fprintf(stderr, "Sorry, you'll have to hack the code. This\n");
      fprintf(stderr, "example does not work for packed 24 bpp\n");
      exit(2);
    }

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,   NULL,
			  EZ_LABEL_STRING,   " ",
			  NULL);
  label = EZ_CreateWidget(EZ_WIDGET_LABEL,  frame,
			  EZ_SIZE,          260,260,
			  EZ_BORDER_WIDTH,  2,
			  NULL);

  ximage = XCreateImage(dpy, visual, depth,  /* dpy info */
			ZPixmap,             /* format   */
			0,  NULL,            /* offset, data */
			256, 256,            /* width and height */
			32,                  /* pad  */
			0);                  /* bytes perline */
  bytes_per_line = ximage->bytes_per_line;
  imageData = (char *)malloc(256* bytes_per_line * sizeof(char));
  if(imageData == NULL); /* barf and exit ... */

  ximage->data = imageData;

  EZ_DisplayWidget(frame); 
  window = EZ_GetWidgetWindow(label);
  gc  = EZ_GetWritableGC();

  while(1)
    {
      XSync(dpy, False);
      make_random_image();
      XPutImage(dpy, window, gc, ximage, 0,0, 
		2, 2, 256,256);
      EZ_ServiceEvents();
      EZ_SitForALittleBit(10000);
    }
}

static void make_random_image()
{
  if(depth == 8)
    {
      char *ptr = imageData;
      int i, j,r,g,b,rgb; 
      for(i = 0; i < 255; i++)
	{
	  if(i == 70)
	    for(j = 0; j < 255; j++)  ptr[j] = RGB2Pixel(j,0,0);
	  else if (i == 140)
	    for(j = 0; j < 255; j++)  ptr[j] = RGB2Pixel(0,j,0);
	  else if(i == 210)
	    for(j = 0; j < 255; j++)  ptr[j] = RGB2Pixel(0,0,j);
	  else
	    for(j = 0; j < 255; j++)
	      {
		rgb = rand();
		r =  rgb & 0xff;
		g =  (rgb >>8) & 0xff;
		b =  (rgb >>16)& 0xff;
		ptr[j] = RGB2Pixel(r,g,b);
	      }
	  ptr += bytes_per_line;
	}
    }
  else if(depth <= 16)
    {
      char *ptr = imageData;
      unsigned short *s = (unsigned short *)ptr;
      int i, j,r,g,b,rgb; 
      for(i = 0; i < 255; i++)
	{
	  if(i == 70)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(j,0,0);
	  else if (i == 140)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(0,j,0);
	  else if(i == 210)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(0,0,j);
	  else
	    for(j = 0; j < 255; j++)
	      {
		rgb = rand();
		r =  rgb & 0xff;
		g =  (rgb >>8) & 0xff;
		b =  (rgb >>16)& 0xff;
		s[j] = (unsigned short)RGB2Pixel(r,g,b);
	      }
	  ptr += bytes_per_line;
	  s = (unsigned short *)ptr;
	}      
    }
  else if(depth >= 24) 
    {
      char *ptr = imageData;
      unsigned long *s = (unsigned long *)ptr;
      int i, j,r,g,b,rgb;
      for(i = 0; i < 255; i++)
	{
	  if(i == 70)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(j,0,0);
	  else if (i == 140)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(0,j,0);
	  else if(i == 210)
	    for(j = 0; j < 255; j++)  s[j] = RGB2Pixel(0,0,j);
	  else
	    for(j = 0; j < 255; j++)
	      {
		rgb = rand();
		r =  rgb & 0xff;
		g =  (rgb >>8) & 0xff;
		b =  (rgb >>16)& 0xff;
		s[j] = (unsigned long)RGB2Pixel(r,g,b);
	      }
	  ptr += bytes_per_line;
	  s = (unsigned long *)ptr;
	}      
    }
}
